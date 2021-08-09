# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Extracts n-gram counts from a monolingual corpus.

print_desc()
{
    echo "thot_pbs_get_ngram_counts written by Daniel Ortiz"
    echo "thot_pbs_get_ngram_counts extracts n-grams counts from a monolingual corpus"
    echo "type \"thot_pbs_get_ngram_counts --help\" to get usage information"
}

version()
{
    echo "thot_pbs_get_ngram_counts is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

usage()
{
    echo "thot_pbs_get_ngram_counts -pr <int>"
    echo "                          -c <string> -o <string> -n <int> [-f <int>] [-unk]"
    echo "                          [-qs <string>] [-tdir <string>] [-sdir <string>]"
    echo "                          [--sync-dep] [-debug] [--help] [--version]"
    echo ""
    echo "-pr <int>          : Number of processors."
    echo "-c <string>        : Corpus file (give absolute path when"
    echo "                     using pbs clusters)."
    echo "-o <string>        : Prefix of output files."
    echo "-n <int>           : Order of the n-grams."
    echo "-f <int>           : Size in lines of the fragments in which the corpus"
    echo "                     is divided when performing map-reduce (50K by default)."
    echo "-unk               : Reserve probability mass for the unknown word."
    echo "-qs <string>       : Specific options to be given to the qsub command"
    echo "                     (example: -qs \"-l pmem=1gb\")."
    echo "-tdir <string>     : Directory for temporary files."
    echo "                     NOTES:"
    echo "                      a) give absolute paths when using pbs clusters."
    echo "                      b) ensure there is enough disk space in the partition."
    echo "-sdir <string>     : Absolute path of a directory common to all"
    echo "                     processors. If not given, \$HOME will be used."
    echo "                     NOTES:"
    echo "                      a) give absolute paths when using pbs clusters."
    echo "                      b) ensure there is enough disk space in the partition."
    echo "--sync-dep         : Use qsub-defined job dependencies to synchronize processes"
    echo "                     (only for pbs clusters). Currently, the implementation"
    echo "                     still has portability issues."
    echo "-debug             : After ending, do not delete temporary files"
    echo "                     (for debugging purposes)."
    echo "--help             : Display this help and exit."
    echo "--version          : Output version information and exit."
}

disabled_pipe_fail()
{
    return $?
}

pipe_fail()
{
    # test if there is at least one command to exit with a non-zero status
    for pipe_status_elem in ${PIPESTATUS[*]}; do 
        if test ${pipe_status_elem} -ne 0; then 
            return 1; 
        fi 
    done
    return 0
}

set_tmp_dir()
{
    if [ -d ${tdir} ]; then
        TMP=${tdir}
    else
        echo "Error: temporary directory does not exist" >&2
        return 1;
    fi
}

set_shared_dir()
{
    SDIR="${sdir}/thot_pbs_get_ngram_counts_sdir_${PPID}_$$"
    mkdir $SDIR || { echo "Error: shared directory cannot be created" ; return 1; }

    # Create temporary subdirectories
    fragms_dir=$SDIR/fragms
    scripts_dir=$SDIR/scripts
    counts_per_fragm_dir=$SDIR/counts_per_fragm
    sync_info_dir=$SDIR/sync
    mkdir ${fragms_dir} || return 1
    mkdir ${scripts_dir} || return 1
    mkdir ${counts_per_fragm_dir} || return 1
    mkdir ${sync_info_dir} || return 1

    # Function executed correctly
    return 0
}

replace_first_word_occurrence_by_unk()
{
    ${AWK} '{
             for(i=1;i<=NF;++i)
             {
              if($i in vocab)
              {
               printf"%s",$i
              }
              else
              {
               vocab[$i]=1
               printf"<unk>"
              }
              if(i!=NF) printf" "
             }
             printf"\n" 
            }' 
}

process_unk_opt()
{
    if [ ${unk_given} -eq 1 ]; then
        TMPF_PCORPUS=`${MKTEMP} $tdir/pcorpus.XXXXXX`
        trap "rm ${TMPF_PCORPUS} ${TMPF_HIST_INFO} 2>/dev/null" EXIT
        
        cat $corpus | replace_first_word_occurrence_by_unk > ${TMPF_PCORPUS} || return 1
        proc_corpus=${TMPF_PCORPUS}
    else
        proc_corpus=$corpus
    fi
}

split_input()
{
    echo "*** Splitting input: ${corpus}..." >> $SDIR/log

    # Determine fragment size
    local input_size=`wc -l ${corpus} 2>/dev/null | ${AWK} '{printf"%d",$1}'`
    if [ ${input_size} -eq 0 ]; then
        echo "Error: input file ${corpus} is empty"
        exit 1
    fi

    if [ ${input_size} -lt ${pr_val} ]; then
        echo "Error: problem too small"
        exit 1
    fi
    _fragm_size=`expr ${input_size} / ${pr_val}`
    _fragm_size=`expr ${_fragm_size} + 1`

    # Split input 
    ${SPLIT} -l ${_fragm_size} ${proc_corpus} ${fragms_dir}/fragm\_ || return 1
}

remove_temp()
{
    # remove shared directory
    if [ "$debug" -eq 0 ]; then
        rm -rf $SDIR 2>/dev/null
    fi
}

exclude_readonly_vars()
{
    ${AWK} -F "=" 'BEGIN{
                         readonlyvars["BASHOPTS"]=1
                         readonlyvars["BASH_VERSINFO"]=1
                         readonlyvars["EUID"]=1
                         readonlyvars["PPID"]=1
                         readonlyvars["SHELLOPTS"]=1
                         readonlyvars["UID"]=1
                        }
                        {
                         if(!($1 in readonlyvars)) printf"%s\n",$0
                        }'
}

exclude_bashisms()
{
    "$AWK" '{if(index($1,"=(")==0) printf"%s\n",$0}'
}

write_functions()
{
    for f in `${AWK} '{if(index($1,"()")!=0) printf"%s\n",$1}' $0`; do
        $SED -n /^$f/,/^}/p $0
    done
}

create_script()
{
    # Init variables
    local name=$1
    local command=$2

    # Write environment variables
    set | exclude_readonly_vars | exclude_bashisms > ${name}

    # Write functions if necessary
    $GREP "()" ${name} -A1 | $GREP "{" > /dev/null || write_functions >> ${name}

    # Write PBS directives
    stream_fname=`${BASENAME} ${name}`
    echo "#PBS -o ${stream_fname}.o\${PBS_JOBID}" >> ${name}
    echo "#PBS -e ${stream_fname}.e\${PBS_JOBID}" >> ${name}
    echo "#$ -cwd" >> ${name}
#    echo "#$ -o ${stream_fname}.o\$JOB_ID" >> ${name}
#    echo "#$ -e ${stream_fname}.e\$JOB_ID" >> ${name}

    # Write command to be executed
    echo "${command}" >> ${name}

    # Give execution permission
    chmod u+x ${name}
}

launch()
{
    local job_deps=$1
    local program=$2
    local suffix=$3
    local outvar=$4

    if [ "${QSUB_WORKS}" = "no" ]; then
        $program &
        eval "${outvar}=$!"
    else
        # Check if the sleep command is used to synchronize processes
        if [ ${sync_sleep} -eq 1 ]; then
            # The sleep command is being used
            # Create script
            create_script ${scripts_dir}/${program}${suffix}.sh $program
            # Execute qsub command
            local jid=$(${QSUB} ${QSUB_TERSE_OPT} ${qs_opts} ${scripts_dir}/${program}${suffix}.sh | ${TAIL} -1)

            # Set value of output variable
            eval "${outvar}='${jid}'"
        else
            # Synchronization is carried out by explicitly defining
            # dependencies between jobs when executing qsub
            
            # Create script
            create_script ${scripts_dir}/${program}${suffix}.sh $program

            # Define qsub option declaring job dependencies
            local depend_opt=""
            if [ ! -z "$job_deps" ]; then
                job_deps=`echo ${job_deps} | "$AWK" '{for(i=1;i<NF;++i) printf"%s:",$i; printf"%s",$NF}'`
                depend_opt="-W depend=afterok:${job_deps}"
            fi
            
            # Execute qsub command. The -h option is used to hold
            # jobs. All jobs are released at the end of the script. This
            # ensures that job dependencies are defined over existing
            # jobs
            local jid=$(${QSUB} ${QSUB_TERSE_OPT} -h ${depend_opt} ${qs_opts} ${scripts_dir}/${program}${suffix}.sh | ${TAIL} -1)

            # Set value of output variable
            eval "${outvar}='${jid}'"

            # Uncomment line to show debug information
            # echo $program ${depend_opt} $jid
        fi
    fi
}

job_is_unknown()
{
    nl=`$QSTAT ${QSTAT_J_OPT} ${jid} 2>&1 | $GREP -e "Unknown" -e "do not exist" | wc -l`
    if [ $nl -ne 0 ]; then
        echo 1
    else
        echo 0
    fi
}

pbs_sync()
{
    # Init vars
    local job_ids=$1
    local pref=$2
    local sync_num_files=`echo "${job_ids}" | "$AWK" '{printf"%d",NF}'`

    if [ ${sync_sleep} -eq 1 ]; then
        # Execute sync loop
        local sync_end=0
        while [ ${sync_end} -ne 1 ]; do
            sleep 2
            
            # Compare current number of sync files written with the required
            # number
            sync_curr_num_files=`ls -l ${sync_info_dir}/ | $GREP " ${pref}" | wc -l`
            if [ ${sync_curr_num_files} -eq ${sync_num_files} ]; then
                sync_end=1
            fi
      
            # Sanity check
            # In pbs clusters, check if there are terminated processes
            # that have not written the sync file
            num_running_procs=0
            for jid in ${job_ids}; do
                job_unknown=`job_is_unknown ${jid}`
                if [ ${job_unknown} -eq 0 ]; then
                    num_running_procs=`expr ${num_running_procs} + 1`
                fi
            done
            if [ ${num_running_procs} -eq 0 ]; then
                sync_curr_num_files=`ls -l ${sync_info_dir}/ | $GREP " ${pref}" | wc -l`
                if [ ${sync_curr_num_files} -ne ${sync_num_files} ]; then
                    return 1
                fi
            fi
        done
      
        return 0
    else
        # No sync loop is required
        return 0
    fi
}

all_procs_ok()
{
    # Init variables
    local job_ids=$1
    local pref=$2
    local sync_num_files=`echo "${job_ids}" | "$AWK" '{printf"%d",NF}'`

    # Obtain number of processes that terminated correctly
    local sync_curr_num_files=`ls -l ${sync_info_dir}/ | $GREP " ${pref}" | wc -l`

    # Return result
    if [ ${sync_num_files} -eq ${sync_curr_num_files} ]; then
        echo "1"
    else
        echo "0"
    fi
}

sync()
{
    # Init vars
    local job_ids=$1
    local pref=$2

    if [ "${QSUB_WORKS}" = "no" ]; then
        wait
        sync_ok=`all_procs_ok "${job_ids}" $pref`
        if [ $sync_ok -eq 1 ]; then
            return 0
        else
            return 1
        fi
    else
        pbs_sync "${job_ids}" $pref
    fi
}

release_job_holds()
{
    job_ids=$1
    ${QRLS} ${job_ids}
    
    # Uncomment line to get debugging information
    # echo ${job_id_list}
}

add_length_col()
{
    ${AWK} '{printf"%d %s\n",NF,$0}'
}

remove_length_col()
{
    ${AWK} '{for(i=2;i<=NF-1;++i)printf"%s ",$i; printf"%d\n",$NF}'
}

sort_counts()
{
    # Set sort command options
    if test ${sortT} = "yes"; then
        SORT_TMP="-T $TMP"
    else
        SORT_TMP=""
    fi

    LC_ALL=C ${SORT} ${SORT_TMP} -t " " ${sortpars}
}

add_fragm_id()
{
    ${AWK} -v c=${fragm_id} '{printf"%s %s\n",$0,c}'    
}

proc_fragm()
{
    # Write date to log file
    echo "** Processing fragm ${fragm} (started at "`date`")..." >> $SDIR/log
    echo "** Processing fragm ${fragm} (started at "`date`")..." > ${counts_per_fragm_dir}/${fragm}_sorted_counts.log

    # Extract counts from fragm and sort them
    $bindir/thot_get_ngram_counts_mr -c ${fragms_dir}/${fragm} -n ${n_val} -f ${fragm_size} -tdir $TMP | add_length_col | \
        sort_counts | add_fragm_id 2>> ${counts_per_fragm_dir}/${fragm}_sorted_counts.log \
        > ${counts_per_fragm_dir}/${fragm}_sorted_counts ; ${PIPE_FAIL} || \
        { echo "Error while executing proc_fragm for ${fragms_dir}/${fragm}" >> $SDIR/log ; return 1 ; }

    # Write date to log file
    echo "Processing of fragm ${fragm} finished ("`date`")" >> $SDIR/log 

    # Create sync file
    echo "" > ${sync_info_dir}/proc_fragm_${fragm}

    return 0
}

merge_sort()
{
    # Set sort command options
    export LC_ALL=""
    export LC_COLLATE=C
    if test ${sortT} = "yes"; then
        SORT_TMP="-T $TMP"
    else
        SORT_TMP=""
    fi

    LC_ALL=C ${SORT} ${SORT_TMP} -t " " ${sortpars} -m ${counts_per_fragm_dir}/*_sorted_counts
}

generate_counts_file()
{
    # Write date to log file
    echo "** Generating counts file (started at "`date`")..." >> $SDIR/log
    echo "** Generating counts file (started at "`date`")..." > ${counts_per_fragm_dir}/gen_counts.log

    # Delete output file if exists
    if [ -f ${output} ]; then
        rm ${output}
    fi

    # Merge count files
    merge_sort 2>> ${counts_per_fragm_dir}/gen_counts.log | \
        remove_length_col 2>> ${counts_per_fragm_dir}/gen_counts.log | \
        ${bindir}/thot_merge_ngram_counts 2>> ${counts_per_fragm_dir}/gen_counts.log \
        > ${output} ; ${PIPE_FAIL} || \
        { echo "Error while executing generate_counts_file" >> $SDIR/log ; return 1 ; }

    # Copy log file
    echo "**** Parallel process finished at: "`date` >> $SDIR/log
    cp $SDIR/log ${output}.getng_log

    # Create sync file
    echo "" > ${sync_info_dir}/generate_counts_file
}

gen_log_err_files()
{
    if [ ${sync_sleep} -eq 1 ]; then
        if [ -f $SDIR/log ]; then
            cp $SDIR/log ${output}.getng_log
        fi

        if [ -f ${output}.getng_err ]; then
            rm ${output}.getng_err
        fi

        for f in ${counts_per_fragm_dir}/*_sorted_counts.log; do
            cat $f >> ${output}.getng_err
        done

        if [ -f ${counts_per_fragm_dir}/gen_counts.log ]; then
            cat $f >> ${output}.getng_err
        fi
    fi
}

report_errors()
{
    if [ ${sync_sleep} -eq 1 ]; then
        num_err=`$GREP "Error while executing" ${output}.getng_log | wc -l`
        if [ ${num_err} -gt 0 ]; then
            # Print error messages
            prog=`$GREP "Error while executing" ${output}.getng_log | head -1 | "$AWK" '{printf"%s",$4}'`
            echo "Error during the execution of thot_get_ngram_counts (${prog})" >&2
            if [ -f ${output}.getng_err ]; then
                echo "File ${output}.getng_err contains information for error diagnosing" >&2
            fi
        else
            echo "Synchronization error" >&2
            if [ -f ${output}.getng_err ]; then
                echo "File ${output}.getng_err contains information for error diagnosing" >&2
            fi
        fi
    fi
}

# main
pr_given=0
unk_given=0
sdir=$HOME
qs_given=0
c_given=0
n_given=0
f_given=0
fragm_size=50000
o_given=0
tdir="/tmp"
sync_sleep=1
debug=0

if [ $# -eq 0 ]; then
    print_desc
    exit 1
fi

while [ $# -ne 0 ]; do
    case $1 in
        "--help") usage
            exit 0
            ;;
        "--version") version
            exit 0
            ;;
        "-pr") shift
            if [ $# -ne 0 ]; then
                pr_val=$1
                pr_given=1
            fi
            ;;
        "-sdir") shift
            if [ $# -ne 0 ]; then
                sdir=$1                
            fi
            ;;
        "-c") shift
            if [ $# -ne 0 ]; then
                corpus=$1
                c_given=1
            else
                c_given=0
            fi
            ;;
        "-n") shift
            if [ $# -ne 0 ]; then
                n_val=$1
                n_given=1
            else
                n_given=0
            fi
            ;;
        "-f") shift
            if [ $# -ne 0 ]; then
                fragm_size=$1
                f_given=1
            else
                f_given=0
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                output=$1
                o_given=1
            else
                o_given=0
            fi
            ;;
        "-qs") shift
            if [ $# -ne 0 ]; then
                qs_opts=$1
                qs_given=1
            else
                qs_given=0
            fi
            ;;
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1
            else
                tdir="/tmp"
            fi
            ;;
        "-unk") unk_given=1
            ;;
        "-debug") debug=1
            debug_opt="-debug"
            ;;
        # "--sync-sleep") sync_sleep=1
        #     ;;
        "--sync-dep") sync_sleep=0
            ;;
    esac
    shift
done

# verify parameters

if [ ${c_given} -eq 0 ]; then
    echo "Error: corpus file not given" >&2
    exit 1
else
    if [ ! -f  "${corpus}" ]; then
        echo "Error: file ${corpus} with training sentences does not exist" >&2
        exit 1
    fi
fi

if [ ${n_given} -eq 0 ]; then
    echo "Error: order of the n-grams not provided" >&2
    exit 1
fi

if [ ${pr_given} -eq 0 ]; then
    # invalid parameters 
    echo "Error: number of processors must be given" >&2
    exit 1
fi

if [ ${o_given} -eq 0 ]; then
    echo "Error: prefix of output files not provided" >&2
    exit 1
fi

# parameters are ok

# Set TMP directory
set_tmp_dir || exit 1

# Set shared directory (global variables are declared)
declare fragms_dir="" 
declare counts_per_fragm_dir="" 
declare scripts_dir="" 

set_shared_dir || exit 1

# Create log file
echo "**** Parallel process started at: "`date` > $SDIR/log

# Process -unk option
process_unk_opt || exit 1

# Split shuffled input into fragms and process them separately...
# job_deps=""
split_input || exit 1

# Declare job id list variable
declare job_id_list=""

# Extract counts from fragm
fragm_id=0
for i in `ls ${fragms_dir}/fragm\_*`; do
    # Initialize variables
    fragm=`${BASENAME} $i`
    fragm_id=`expr $fragm_id + 1`
        
    # Process fragment
    job_deps=""
    launch "${job_deps}" proc_fragm "_${fragm_id}" job_id || exit 1
    pc_job_ids=${job_id}" "${pc_job_ids}
done

# Extract counts synchronization
sync "${pc_job_ids}" "proc_fragm" || { gen_log_err_files ; report_errors ; exit 1; }

# Generate counts file
job_deps=${pc_job_ids}
launch "${job_deps}" generate_counts_file "" gcf_job_id || exit 1;

# Generate counts file synchronization
sync "${gcf_job_id}" "generate_counts_file" || { gen_log_err_files ; report_errors ; exit 1; }

# Generate log and err files
gen_log_err_files

# Remove temporary files
if [ ${sync_sleep} -eq 1 ]; then
    # Sync using sleep is enabled
    remove_temp
else
    # Sync using sleep is not enabled
    job_deps=${gcf_job_id}
    launch "${job_deps}" remove_temp "" rt_job_id || exit 1;
    # Remove temporary files synchronization
    sync "${rt_job_id}" "remove_temp" || { report_errors ; exit 1; }
fi

# Update job_id_list
job_id_list="${pc_job_ids} ${gcf_job_id} ${rt_job_id}"

# Release job holds
if [ ! "${QSUB_WORKS}" = "no" -a ${sync_sleep} -eq 0 ]; then
    release_job_holds "${job_id_list}"
fi
