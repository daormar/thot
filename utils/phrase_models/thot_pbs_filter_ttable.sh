# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Filters a translation table given a test corpus and an integer value
# that sets the maximum number of translation options for each target
# phrase that are considered during a translation process. The utility
# can be executed in a PBS cluster.

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

#############
filter_ttable()
{
    $bindir/thot_filter_ttable_given_corpus ${ttable_file} ${test_corpus_file} |\
        $bindir/thot_get_nbest_for_trg -n ${n_val} -p -T $TMP > $outfile

    echo "" > $SDIR/filter_ttable_end
}


#############
launch()
{
    local file=$1
    ### qsub invocation
    if [ "${QSUB_WORKS}" = "no" ]; then
        $file &
    else
        local jid=$($QSUB ${QSUB_TERSE_OPT} ${qs_opts} $file | ${TAIL} -1)
    fi
    ###################
}

#############
sync()
{
    local files="$1"
    end=0
    while [ $end -ne 1 ]; do
        sleep 3
        end=1
        for f in ${files}; do
            if [ ! -f ${f}_end ]; then
                end=0
                break
            fi
        done
    done
}

#############
N_DEFAULT=20
if [ $# -eq 0 ]; then
    echo "Usage: thot_pbs_filter_ttable -t <string> -c <string>" >&2
    echo "                              -o <string> [-n <int>] [-qs <string>]" >&2
    echo "                              [-sdir <string>] [-T <string>] [-debug]" >&2
    echo "" >&2
    echo "-t <string>         : Thot translation table" >&2
    echo "-c <string>         : Test corpus to be translated" >&2
    echo "-o <string>         : Output file" >&2
    echo "-n <int>            : Maximum number of translation options for each target" >&2
    echo "                      phrase that are considered during a translation process" >&2
    echo "                      ("${N_DEFAULT}" by default)" >&2
    echo "-qs <string>        : Specific options to be given to the qsub command"
    echo "                      (example: -qs \"-l pmem=1gb\")"
    echo "-sdir <string>      : Absolute path of a directory common to all" >&2
    echo "                      processors. If not given, \$HOME will be used" >&2
    echo "-T <string>         : Use <string> for temporaries instead of /tmp" >&2
    echo "                      during the generation of the phrase model" >&2
    echo "-debug              : After ending, do not delete temporary files" >&2
    echo "                      (for debugging purposes)" >&2
else
    t_given=0
    ttable_file=""
    c_given=0
    o_given=0
    n_val=${N_DEFAULT}
    tmpdir="/tmp"
    sdir=$HOME
    debug=0
    while [ $# -ne 0 ]; do
        case $1 in
            "-t") shift
                if [ $# -ne 0 ]; then
                    ttable_file=$1
                    t_given=1
                fi
                ;;
            "-c") shift
                if [ $# -ne 0 ]; then
                    test_corpus_file=$1
                    c_given=1
                fi
                ;;
            "-o") shift
                if [ $# -ne 0 ]; then
                    outfile=$1
                    o_given=1
                fi
                ;;
            "-n") shift
                if [ $# -ne 0 ]; then
                    n_val=$1
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
            "-sdir") shift
                if [ $# -ne 0 ]; then
                    sdir=$1                
                fi
                ;;
            "-T") shift
                if [ $# -ne 0 ]; then
                    tmpdir=$1
                else
                    tmpdir="/tmp"
                fi
                ;;
            "-debug") debug=1
                ;;
        esac
        shift
    done

    # verify parameters

    if [ ${t_given} -eq 0 ]; then
        echo "Error: -t option not given" >&2
        exit 1        
    else
        if [ ! -f ${ttable_file} ]; then
            echo "Error: file ${ttable_file} does not exist!" >&2
            exit 1
        fi        
    fi

    if [ ${o_given} -eq 0 ]; then
        echo "Error: -o option not given" >&2
        exit 1
    fi

    if [ ${c_given} -eq 0 ]; then
        echo "Error: -c option not given!"
        exit 1
    else
        if [ ! -f ${test_corpus_file} ]; then
            echo "Error: file ${test_corpus_file} does not exist!" >&2
            exit 1
        fi
    fi

    # create TMP directory
    TMP="${tmpdir}/thot_pbs_filter_ttable_tmp_$$"
    mkdir $TMP || { echo "Error: temporary directory cannot be created" >&2 ; exit 1; }

    # create shared directory
    SDIR="${sdir}/thot_pbs_filter_ttable_sdir_$$"
    mkdir $SDIR || { echo "Error: shared directory cannot be created" >&2 ; exit 1; }
    
    # remove temp directories on exit
    if [ $debug -eq 0 ]; then
        trap "rm -rf $TMP $SDIR 2>/dev/null" EXIT
    fi

    # process the input

    # filter table
    create_script $SDIR/filter_ttable filter_ttable
    launch $SDIR/filter_ttable
    
    ### Check that all queued jobs are finished
    sync $SDIR/filter_ttable
fi
