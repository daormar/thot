# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# alig_op using pbs clusters

print_desc()
{
    echo "thot_pbs_alig_op written by Daniel Ortiz"
    echo "thot_pbs_alig_op is a parallelized version of thot_alig_op for pbs clusters"
    echo "type \"thot_pbs_alig_op --help\" to get usage information."
}

version()
{
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

usage()
{
    echo "Usage: thot_pbs_alig_op {-pr <int>} {-g <string>}"
    echo "                   {<-and|-or|-sum|-sym1|-sym2|-grd> <string>}"
    echo "                   {-o <string>} [-qs <string>]"
    echo "                   [-sdir <string>] [-T <string>]"
    echo "                   [-debug] [--help] [--version]"
    echo ""
    echo "-pr <int>                       Number of processors."
    echo ""
    echo "-g <string>                     Name of the alignment file in GIZA format."
    echo ""
    echo "-<and|or|sum|sym1|sym2|grd> <string>"
    echo "                                Operation to be executed, using <string> as"
    echo "                                the second file operand."
    echo ""
    echo "-o <string>                     Set output files prefix name."
    echo ""
    echo "-qs <string>                    Specific options to be given to the qsub"
    echo "                                command (example: -qs \"-l pmem=1gb\")."
    echo ""
    echo "-sdir <string>                  Absolute path of a directory common to all"
    echo "                                processors. If not given, \$HOME will be used."
    echo ""
    echo "-T <string>                     Use <string> for temporaries instead of /tmp"
    echo ""
    echo "-debug                          After ending, do not delete temporary files"
    echo "                                (for debugging purposes)"
    echo ""
    echo "--help                          Display this help and exit"
    echo ""
    echo "--version                       Output version information and exit"
    echo ""
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
    $AWK '{if(index($1,"=(")==0) printf"%s\n",$0}'
}

write_functions()
{
    for f in `${AWK} '{if(index($1,"()")!=0) printf"%s\n",$1}' $0`; do
        $SED -n /^$f/,/^}/p $0
        # $AWK -v f=$f '{
        #                if($1==f)
        #                {
        #                 printf"%s\n",$0
        #                 found=1
        #                }
        #                else
        #                {
        #                 if(found) printf"%s\n",$0
        #                 if($1=="}") found=0
        #                }
        #               }' $0
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
    echo "#PBS -o ${name}.o\${PBS_JOBID}" >> ${name}
    echo "#PBS -e ${name}.e\${PBS_JOBID}" >> ${name}
    echo "#$ -cwd" >> ${name}

    # Write command to be executed
    echo "${command}" >> ${name}

    # Give execution permission
    chmod u+x ${name}
}

alig_op_frag()
{
    echo "** Processing chunk ${fragm} (started at "`date`")..." >> $SDIR/log
    echo "** Processing chunk ${fragm} (started at "`date`")..." > $SDIR/${fragm}_proc.log

    $bindir/thot_alig_op -g $SDIR/${fragm} ${operation} $SDIR/op_file_${fragm} \
        -o $SDIR/${fragm} 2>> $SDIR/${fragm}_proc.log || \
        { echo "Error while executing alig_op_frag for $SDIR/${fragm}" >> $SDIR/log; return 1 ; }

    # Write date to log file
    echo "Processing of chunk ${fragm} finished ("`date`")" >> $SDIR/log

    echo "" > $SDIR/qs_alig_${fragm}_end
}

merge_alig_op()
{
    echo "** Merging alignment files (started at "`date`")..." >> $SDIR/log
    echo "** Merging alignment files (started at "`date`")..." > $SDIR/merge.log

    # merge alig files
    cat $SDIR/*.A3.final 2>> $SDIR/merge.log > ${output}.A3.final || \
        { echo "Error while executing merge_alig_op" >> $SDIR/log ; return 1 ; }

    # Write date to log file
    echo "Merging process finished ("`date`")" >> $SDIR/log
    
    echo "" > $SDIR/merge_alig_op_end
}

launch()
{
    local file=$1
    local outvar=$2

    ### qsub invocation
    if [ "${QSUB_WORKS}" = "no" ]; then
        $file &
        eval "${outvar}=$!"
    else
        local jid=$($QSUB ${QSUB_TERSE_OPT} ${qs_opts} $file | ${TAIL} -1)
        eval "${outvar}='${jid}'"
    fi
    ###################
}

all_procs_ok()
{
    # Init variables
    local files="$1"
    local sync_num_files=`echo "${files}" | $AWK '{printf"%d",NF}'`    
    local sync_curr_num_files=0

    # Obtain number of processes that terminated correctly
    for f in ${files}; do
        if [ -f ${f}_end ]; then
            sync_curr_num_files=`expr ${sync_curr_num_files} + 1`
        fi
    done

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
    local files="$1"
    local job_ids="$2"

    if [ "${QSUB_WORKS}" = "no" ]; then
        wait
        sync_ok=`all_procs_ok "$files"`
        if [ $sync_ok -eq 1 ]; then
            return 0
        else
            return 1
        fi
    else
        pbs_sync "$files" "${job_ids}"
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
    local files="$1"
    local job_ids="$2"
    local num_pending_procs=0
    end=0
    while [ $end -ne 1 ]; do
        sleep 3
        end=1
        # Check if all processes have finished
        for f in ${files}; do
            if [ ! -f ${f}_end ]; then
                num_pending_procs=`expr ${num_pending_procs} + 1`
                end=0
            fi
        done

        # Sanity check
        num_running_procs=0
        for jid in ${job_ids}; do
            job_unknown=`job_is_unknown ${jid}`
            if [ ${job_unknown} -eq 0 ]; then
                num_running_procs=`expr ${num_running_procs} + 1`
            fi
        done
        if [ ${num_running_procs} -eq 0 -a ${num_pending_procs} -ge 1 ]; then
            return 1
        fi
    done
}

gen_log_err_files()
{
    # Copy log file to its final location
    if [ -f $SDIR/log ]; then
        cp $SDIR/log ${output}.aligop_log
    fi

    # Generate file for error diagnosing
    if [ -f ${output}.aligop_err ]; then
        rm ${output}.aligop_err
    fi
    for f in $SDIR/*_proc.log; do
        cat $f >> ${output}.aligop_err
    done
    if [ -f $SDIR/merge.log ]; then
        cat $SDIR/merge.log >> ${output}.aligop_err
    fi
}

report_errors()
{
    num_err=`$GREP "Error while executing" ${output}.aligop_log | wc -l`
    if [ ${num_err} -gt 0 ]; then
        prog=`$GREP "Error while executing" ${output}.aligop_log | head -1 | $AWK '{printf"%s",$4}'`
        echo "Error during the execution of thot_pbs_alig_op (${prog})" >&2
        echo "File ${output}.aligop_err contains information for error diagnosing" >&2
    else
        echo "Synchronization error" >&2
        echo "File ${output}.aligop_err contains information for error diagnosing" >&2
    fi
}

# main
pr_given=0
g_given=0
o_given=0
op_given=0
qs_given=0
tmpdir="/tmp"
debug=""
sdir=$HOME

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
                num_hosts=$1
                pr_given=1
            fi
            ;;
        "-sdir") shift
            if [ $# -ne 0 ]; then
                sdir=$1
            fi
            ;;
        "-g") shift
            if [ $# -ne 0 ]; then
                a3_file=$1
                g_given=1
            fi
            ;;
        "-and") shift
            if [ $# -gt 0 ]; then
                operation="-and"
                op_file=$1
                op_given=1
            fi
            ;;
        "-or") shift
            if [ $# -gt 0 ]; then
                operation="-or"
                op_file=$1
                op_given=1
            fi
            ;;
        "-sum") shift
            if [ $# -gt 0 ]; then
                operation="-sum"
                op_file=$1
                op_given=1
            fi
            ;;
        "-sym1") shift
            if [ $# -gt 0 ]; then
                operation="-sym1"
                op_file=$1
                op_given=1
            fi
            ;;
        "-sym2") shift
            if [ $# -gt 0 ]; then
                operation="-sym2"
                op_file=$1
                op_given=1
            fi
            ;;
        "-grd") shift
            if [ $# -gt 0 ]; then
                operation="-grd"
                op_file=$1
                op_given=1
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                output=$1
                o_given=1
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
        "-debug") debug="-debug"
            ;;
        "-T") shift
            if [ $# -ne 0 ]; then
                tmpdir=$1
            else
                tmpdir="/tmp"
            fi
            ;;
    esac
    shift
done

# verify parameters
if [ ${g_given} -eq 1 ]; then
    # verify that -g file exist
    if [ ${g_given} -eq 1 -a  ! -f  "${a3_file}" ];then
        echo "Error: file "$a3_file" does not exist " >&2
        exit 1
    fi
else
    echo "Error: -g option has to be given" >&2
    exit 1
fi

if [ ${o_given} -eq 0 ];then
    # invalid parameters 
    echo "Error: -o option has to be given" >&2
    exit 1
fi

if [ ${op_given} -eq 0 ];then
    # invalid parameters 
    echo "Error: <-and|-or|-sum|-sym1|-sym2|grd>  option has to be given" >&2
    exit 1
fi

if [ ${pr_given} -eq 0 ]; then
    # invalid parameters 
    echo "Error: number of processors not given" >&2
    exit 1
fi

# parameters are ok

# create TMP directory
TMP="${tmpdir}/thot_pbs_alig_op_tmp_$$"
mkdir $TMP || { echo "Error: temporary directory cannot be created"  >&2 ; exit 1; }

# create shared directory
SDIR="${sdir}/thot_pbs_alig_op_sdir_$$"
mkdir $SDIR || { echo "Error: shared directory cannot be created"  >&2 ; exit 1; }
    
# remove temp directories on exit
if [ "$debug" != "-debug" ]; then
    trap "rm -rf $TMP $SDIR 2>/dev/null" EXIT
fi

# Output info about tracking script progress
echo "NOTE: see file $SDIR/log to track matrix operation progress" >&2

# create log file
echo "*** Parallel process started at: " `date` > $SDIR/log
echo "">> $SDIR/log

# process the input

# fragment the input
echo "Spliting input: ${a3_file}..." >> $SDIR/log
input_size=`wc ${a3_file} 2>/dev/null | ${AWK} '{printf"%d",$(1)/3}'`
if [ ${input_size} -eq 0 ]; then
    echo "Error: input file ${a3_file} is empty" >&2
    exit 1
fi

if [ ${input_size} -lt ${num_hosts} ]; then
    echo "Error: problem too small"  >&2
    exit 1
fi
frag_size=`expr ${input_size} / ${num_hosts}`
frag_size=`expr ${frag_size} + 1`
nlines=`expr ${frag_size} \* 3`
${SPLIT} -l ${nlines} $a3_file $SDIR/frag\_ || exit 1
${SPLIT} -l ${nlines} ${op_file} $SDIR/op_file_frag\_ || exit 1

# parallel estimation for each fragment
i=1
qs_alig=""
jids=""
for f in `ls $SDIR/frag\_*`; do
    fragm=`${BASENAME} $f`

    create_script $SDIR/qs_alig_${fragm} alig_op_frag || exit 1
    launch $SDIR/qs_alig_${fragm} job_id || exit 1
    qs_alig="${qs_alig} $SDIR/qs_alig_${fragm}"
    jids="${jids} ${job_id}"

    i=`expr $i + 1`
done

### Check that all queued jobs are finished
sync "${qs_alig}" "${jids}" || { gen_log_err_files ; report_errors ; exit 1; }

# merge counts and files

create_script $SDIR/merge_alig_op merge_alig_op || exit 1
launch $SDIR/merge_alig_op job_id || exit 1
    
### Check that all queued jobs are finished
sync $SDIR/merge_alig_op "${job_id}" || { gen_log_err_files ; report_errors ; exit 1; }

# finish log file
echo "">> $SDIR/log
echo "*** Parallel process finished at: " `date` >> $SDIR/log

# Generate log and err files
gen_log_err_files
