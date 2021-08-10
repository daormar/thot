# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Gets the first n source phrases for each target phrase with higher
# value of c_st. If n is between 0 and 1, then n is used as a threshold
# value. Given a target phrase t~, the script will show those
# translations whose probability is higher than n*max_prob, where
# max_prob is the maximum probability of all possible translations for
# t~
# NOTE: here, source phrases are those that appear in first place in
# the phrase table

# INCLUDE BASH LIBRARIES
. "${bindir}"/thot_general_lib || exit 1
. "${bindir}"/thot_simple_sched_lib || exit 1

#############
get_nbest_for_trg()
{
    # Write date to log file
    echo "** Processing file ${ttable_file} (started at "`date`")..." >> "$SDIR"/log

    cat "${ttable_file}" 2> "$SDIR"/err | \
        "$bindir"/thot_get_nbest_for_trg -n ${n_val} -p -T "$TMP" 2>> "$SDIR"/err > "$outfile" ; ${PIPEFAIL} || \
        { echo "Error while executing get_nbest_for_trg" >> "$SDIR"/log ; return 1 ; }

    # Write date to log file
    echo "Processing of file ${ttable_file} finished ("`date`")" >> $SDIR/log 

    echo "" > "$SDIR"/get_nbest_for_trg_end
}

#############
gen_log_err_files()
{
    # Copy log file to its final location
    if [ -f "$SDIR"/log ]; then 
        cp "$SDIR"/log "${outfile}".getnb_log
    fi

    # Generate file for error diagnosing
    if [ -f "$SDIR"/err ]; then
        cp "$SDIR"/err "${outfile}".getnb_err
    fi
}

#############
report_errors()
{
    num_err=`"$GREP" "Error while executing get_nbest_for_trg" "${outfile}".getnb_log | wc -l`
    if [ ${num_err} -gt 0 ]; then
        echo "Error during the execution of thot_pbs_get_nbest_for_trg (get_nbest_for_trg)" >&2
        if [ -f "${outfile}".getnb_err ]; then
            echo "File ${outfile}.getnb_err contains information for error diagnosing" >&2
        fi
    else
        echo "Synchronization error" >&2
        if [ -f "${outfile}".getnb_err ]; then
            echo "File ${outfile}.getnb_err contains information for error diagnosing" >&2
        fi
    fi
}

#############
N_DEFAULT=20
if [ $# -eq 0 ]; then
    echo "Usage: thot_pbs_get_nbest_for_trg -t <string> -o <string>" >&2
    echo "                                  [-n <int>] [-qs <string>]" >&2
    echo "                                  [-sdir <string>] [-T <string>] [-debug]" >&2
    echo "" >&2
    echo "-t <string>         : Thot translation table" >&2
    echo "-o <string>         : Output file" >&2
    echo "-n <int>            : Maximum number of translation options for each target" >&2
    echo "                      phrase that are considered during a translation process" >&2
    echo "                      ("${N_DEFAULT}" by default)." >&2
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
        if [ ! -f "${ttable_file}" ]; then
            echo "Error: file ${ttable_file} does not exist!" >&2
            exit 1
        fi        
    fi

    if [ ${o_given} -eq 0 ]; then
        echo "Error: -o option not given" >&2
        exit 1
    fi

    # create TMP directory
    TMP="${tmpdir}/thot_pbs_get_nbest_for_trg_tmp_$$"
    mkdir "$TMP" || { echo "Error: temporary directory cannot be created" >&2 ; exit 1; }

    # create shared directory
    SDIR="${sdir}/thot_pbs_get_nbest_for_trg_sdir_$$"
    mkdir "$SDIR" || { echo "Error: shared directory cannot be created" >&2 ; exit 1; }
    
    # remove temp directories on exit
    if [ $debug -eq 0 ]; then
        trap 'rm -rf "$TMP" "$SDIR" 2>/dev/null' EXIT
    fi

    # create log file
    echo "*** Parallel process started at: " `date` > "$SDIR"/log
    echo "">> "$SDIR"/log

    # process the input

    # get n-best translations
    create_script "$SDIR"/get_nbest_for_trg get_nbest_for_trg || exit 1
    launch "$SDIR"/get_nbest_for_trg job_id || exit 1
    
    ### Check that all queued jobs are finished
    sync "$SDIR"/get_nbest_for_trg "${job_id}" || { gen_log_err_files ; report_errors ; exit 1; }

    # Add footer to log file
    echo "">> "$SDIR"/log
    echo "*** Parallel process finished at: " `date` >> "$SDIR"/log

    # Generate log and err files
    gen_log_err_files
fi
