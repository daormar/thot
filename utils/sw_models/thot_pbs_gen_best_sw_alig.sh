# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Generates the best single word alignment for the sentence pairs contained in a parallel corpus.

print_desc()
{
    echo "thot_pbs_gen_best_sw_alig written by Daniel Ortiz"
    echo "thot_pbs_gen_best_sw_alig generates the best word alignment for a parallel corpus"
    echo "type \"thot_pbs_gen_best_sw_alig --help\" to get usage information"
}

version()
{
    echo "thot_pbs_gen_best_sw_alig is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

usage()
{
    echo "thot_pbs_gen_best_sw_alig -pr <int> -sw <string>"
    echo "                      -s <string> -t <string> -o <string>"
    echo "                      [-shu] [-qs <string>]"
    echo "                      [-tdir <string>] [-sdir <string>]"
    echo "                      [--sync-sleep] [-debug] [--help] [--version]"
    echo ""
    echo "-pr <int>          : Number of processors."
    echo "-sw <string>       : Prefix of single word model files."
    echo "                     NOTES:"
    echo "                      a) give absolute paths when using pbs clusters."
    echo "                      b) ensure that the given path is reachable by all nodes."
    echo "-s <string>        : File with source sentences (give absolute path when"
    echo "                     using pbs clusters)."
    echo "-t <string>        : File with target sentences (give absolute path when"
    echo "                     using pbs clusters)."
    echo "-o <string>        : Output prefix (give absolute path when"
    echo "                     using pbs clusters)."
    echo "-shu               : Shuffle input files before splitting them."
    echo "-qs <string>       : Specific options to be given to the qsub command"
    echo "                     (example: -qs \"-l pmem=1gb\")."
    echo "-tdir <string>     : Directory for temporary files."
    echo "                     NOTES:"
    echo "                      a) give absolute paths when using pbs clusters."
    echo "                      b) ensure there is enough disk space in the partition."
    echo "-sdir <string>     : Absolute path of a directory common to all"
    echo "                     processors. If not given, the directory for"
    echo "                     temporaries will be used (/tmp or the"
    echo "                     directory given by means of the -tdir option)."
    echo "                     NOTES:"
    echo "                      a) give absolute paths when using pbs clusters."
    echo "                      b) ensure there is enough disk space in the partition."
    echo "--sync-sleep       : Use sleep command loops to synchronize process (only for"
    echo "                     pbs clusters). This is useful when the number of jobs that"
    echo "                     can be executed with qsub is restricted."
    echo "-debug             : After ending, do not delete temporary files"
    echo "                     (for debugging purposes)."
    echo "--help             : Display this help and exit."
    echo "--version          : Output version information and exit."
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
        echo "Error: temporary directory does not exist"
        return 1;
    fi
}

set_shared_dir()
{
    if [ -z "$sdir" ]; then
        # if not given, SDIR will be created in the $TMP directory
        SDIR="${TMP}/thot_pbs_gen_best_sw_alig_sdir_${PPID}_$$"
        mkdir $SDIR || { echo "Error: shared directory cannot be created" ; return 1; }
    else
        SDIR="${sdir}/thot_pbs_gen_best_sw_alig_sdir_${PPID}_$$"
        mkdir $SDIR || { echo "Error: shared directory cannot be created" ; return 1; }
    fi

    # Create temporary subdirectories
    chunks_dir=$SDIR/chunks
    scripts_dir=$SDIR/scripts
    aligs_per_chunk_dir=$SDIR/aligs_per_chunk
    sync_info_dir=$SDIR/sync
    mkdir ${chunks_dir} || return 1
    mkdir ${scripts_dir} || return 1
    mkdir ${aligs_per_chunk_dir} || return 1
    mkdir ${sync_info_dir} || return 1

    # Function executed correctly
    return 0
}

split_input()
{
    echo "*** Shuffling and splitting input: ${srcf} ${trgf}..." >> $SDIR/log

    # Determine fragment size
    local input_size=`wc -l ${srcf} 2>/dev/null | ${AWK} '{printf"%d",$1}'`
    if [ ${input_size} -lt ${pr_val} ]; then
        echo "Error: problem too small"
        exit 1
    fi
    local chunk_size=`expr ${input_size} / ${pr_val}`
    local chunk_size=`expr ${chunk_size} + 1`

    # Shuffle (optional) and split input (shuffling is required to
    # achieve load balancing)
    if [ ${shu_given} -eq 0 ]; then
        ${SPLIT} -l ${chunk_size} ${srcf} ${chunks_dir}/src\_chunk\_ || return 1
        ${SPLIT} -l ${chunk_size} ${trgf} ${chunks_dir}/trg\_chunk\_ || return 1
    else
        local rand_seed=31415
${bindir}/thot_shuffle ${rand_seed} ${srcf} | ${SPLIT} -l ${chunk_size} - ${chunks_dir}/src\_chunk\_ || return 1
${bindir}/thot_shuffle ${rand_seed} ${trgf} | ${SPLIT} -l ${chunk_size} - ${chunks_dir}/trg\_chunk\_ || return 1
    fi
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

create_script()
{
    # Init variables
    local name=$1
    local command=$2

    # Write environment information
    declare | exclude_readonly_vars > ${name}

    # If sh is being used as command interpreter, functions have to be
    # added explicitly using the -f option of the "declare" command
    command_interp=$(basename "${BASH}")
    if [ ${command_interp} = "sh" ]; then
        declare -f >> ${name}
    fi

    # Write PBS directives
    echo "#PBS -o ${name}.o\${PBS_JOBID}" >> ${name}
    echo "#PBS -e ${name}.e\${PBS_JOBID}" >> ${name}

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

    if [ -z "$QSUB" ]; then
        $program &
        eval "${outvar}=$!"
    else
        # Check if the sleep command is used to synchronize processes
        if [ ${sync_sleep} -eq 1 ]; then
            # The sleep command is being used
            # Create script
            create_script ${scripts_dir}/${program}${suffix}.sh $program
            # Execute qsub command
            local jid=$(${QSUB} ${qs_opts} ${scripts_dir}/${program}${suffix}.sh)

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
                job_deps=`echo ${job_deps} | $AWK '{for(i=1;i<NF;++i) printf"%s:",$i; printf"%s",$NF}'`
                depend_opt="-W depend=afterok:${job_deps}"
            fi

            # Execute qsub command. The -h option is used to hold
            # jobs. All jobs are released at the end of the script. This
            # ensures that job dependencies are defined over existing
            # jobs
            local jid=$(${QSUB} -h ${depend_opt} ${qs_opts} ${scripts_dir}/${program}${suffix}.sh)

            # Set value of output variable
            eval "${outvar}='${jid}'"

            # Uncomment line to show debug information
            # echo $program ${depend_opt} $jid
        fi
    fi
}

pbs_sync()
{
    # Init vars
    local job_ids=$1
    local pref=$2
    local sync_num_files=`echo "${job_ids}" | $AWK '{printf"%d",NF}'`

    if [ ${sync_sleep} -eq 1 ]; then
        # Execute sync loop
        local sync_end=0
        while [ ${sync_end} -ne 1 ]; do
            sleep 2
            
            # Compare current number of sync files written with the required
            # number
            sync_curr_num_files=`ls -l ${sync_info_dir}/ | grep " ${pref}" | wc -l`
            if [ ${sync_curr_num_files} -eq ${sync_num_files} ]; then
                sync_end=1
            fi
      
            # Sanity check
            # In pbs clusters, check if there are terminated processes
            # that have not written the sync file
            num_running_procs=0
            for jid in ${job_ids}; do
                nl=`$QSTAT ${jid} 2>&1 | grep "Unknown" | wc -l`
                if [ $nl -eq 0 ]; then
                    num_running_procs=`expr ${num_running_procs} + 1`
                fi
            done
            if [ ${num_running_procs} -eq 0 ]; then
                sync_curr_num_files=`ls -l ${sync_info_dir}/ | grep " ${pref}" | wc -l`
                if [ ${sync_curr_num_files} -ne ${sync_num_files} ]; then
                    echo "Error during synchronization"
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

sync()
{
    # Init vars
    local job_ids=$1
    local pref=$2

    if [ -z "$QSUB" ]; then
        wait
        return 0
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

proc_chunk()
{
    # Write date to log file
    echo "** Processing chunk ${chunk} (started at "`date`")..." >> $SDIR/log

    ${bindir}/thot_format_corpus_csl ${chunks_dir}/${src_chunk} ${chunks_dir}/${trg_chunk} | \
        ${bindir}/thot_calc_swm_lgprob -sw ${sw_val} -P - -max > ${aligs_per_chunk_dir}/${chunk}_bestal 2> ${aligs_per_chunk_dir}/${chunk}_bestal.log ; pipe_fail || return 1

    # Write date to log file
    echo "Processing of chunk ${chunk} finished ("`date`")" >> $SDIR/log 

    # Create sync file
    echo "" > ${sync_info_dir}/proc_chunk_${chunk}

    return 0
}

generate_alig_file()
{
    # Delete output file if exists
    if [ -f ${output}.bestal ]; then
        rm ${output}.bestal
    fi

    # Dump alignments in output file
    for f in `ls ${aligs_per_chunk_dir}/*_bestal`; do
        cat $f >> ${output}.bestal
    done

    # Copy log file
    echo "**** Parallel process finished at: "`date` >> $SDIR/log
    cp $SDIR/log ${output}.log

    # Create sync file
    echo "" > ${sync_info_dir}/generate_alig_file
}

pr_given=0
sw_given=0
shu_given=0
qs_given=0
sdir=""
s_given=0
t_given=0
o_given=0
tdir="/tmp"
sync_sleep=0
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
            else
                sdir=""
            fi
            ;;
        "-sw") shift
            if [ $# -ne 0 ]; then
                sw_val=$1
                sw_given=1
            else
                sw_given=0
            fi
            ;;
        "-s") shift
            if [ $# -ne 0 ]; then
                srcf=$1
                s_given=1
            else
                s_given=0
            fi
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                trgf=$1
                t_given=1
            else
                t_given=0
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
                tdir="./"
            fi
            ;;
        "-shu") shu_given=1
            ;;
        "-debug") debug=1
            ;;
        "--sync-sleep") sync_sleep=1
            ;;
    esac
    shift
done

# verify parameters

if [ ${sw_given} -eq 0 ];then
    # invalid parameters 
    echo "Error: prefix of single word model files must be given"
    exit 1
fi

if [ ${s_given} -eq 0 ]; then
    echo "Error: file with source sentences not given"
    exit 1
else
    if [ ! -f  "${srcf}" ]; then
        echo "Error: file ${srcf} with source sentences does not exist"
        exit 1
    fi
fi

if [ ${t_given} -eq 0 ]; then
    echo "Error: file with target sentences not given"
    exit 1
else
    if [ ! -f  "${trgf}" ]; then
        echo "Error: file ${trgf} with target sentences does not exist"
    fi
fi

if [ ${o_given} -eq 0 ];then
    # invalid parameters 
    echo "Error: output files prefix must be given"
    exit 1
fi

if [ ${pr_given} -eq 0 ]; then
    # invalid parameters 
    echo "Error: number of processors must be given"
    exit 1
fi

# parameters are ok

# Set TMP directory
set_tmp_dir || exit 1

# Set shared directory (global variables are declared)
declare chunks_dir="" 
declare aligs_per_chunk_dir="" 
declare scripts_dir="" 
declare sync_info_dir=""

set_shared_dir || exit 1

# Create log file
echo "**** Parallel process started at: "`date` > $SDIR/log

# Split shuffled input into chunks and process them separately...
# job_deps=""
split_input || exit 1

# Declare job id list variable
declare job_id_list=""

# Generate best alignment
for i in `ls ${chunks_dir}/src\_chunk\_*`; do
    # Initialize variables
    chunk=`${BASENAME} $i`
    chunk=${chunk:4}
    src_chunk="src_"${chunk}
    trg_chunk="trg_"${chunk}
    chunk_id=`expr $chunk_id + 1`
        
    # Process chunk
    job_deps=""
    launch "${job_deps}" proc_chunk "_${chunk_id}" job_id || exit 1
    pc_job_ids=${job_id}" "${pc_job_ids}
done

# Alignment generation synchronization
sync "${pc_job_ids}" "proc_chunk" || exit 1

# Generate alignment file
job_deps=${pc_job_ids}
launch "${job_deps}" generate_alig_file "" gaf_job_id || exit 1

# Generate alignment file synchronization
sync ${gaf_job_id} "generate_alig_file" || exit 1

# Remove temporary files
if [ ${sync_sleep} -eq 1 ]; then
    # Sync using sleep is enabled
    remove_temp
else
    # Sync using sleep is not enabled
    job_deps=${gfm_job_id}
    launch "${job_deps}" remove_temp "" rt_job_id || exit 1
    # Update job_id_list
    job_id_list="${rt_job_id} ${job_id_list}"
    # Remove temporary files synchronization
    sync ${rt_job_id} "remove_temp" || exit 1
fi

# Update job_id_list
job_id_list="${pc_job_ids} ${gaf_job_id} ${rt_job_id}"

# Release job holds
if [ ! -z "${QSUB}" ]; then
    release_job_holds "${job_id_list}"
fi
