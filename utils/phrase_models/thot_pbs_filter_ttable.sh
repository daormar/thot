# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Filters a translation table given a test corpus and an integer value
# that sets the maximum number of translation options for each target
# phrase that are considered during a translation process. The utility
# can be executed in a PBS cluster.

# INCLUDE BASH LIBRARIES
. "${bindir}"/thot_shlib || exit 1

#############
filter_ttable()
{
    # Write date to log file
    echo "** Processing file ${ttable_file} (started at "`date`")..." >> "$SDIR"/log

    "$bindir"/thot_filter_ttable_given_corpus "${ttable_file}" "${test_corpus_file}" 2> "$SDIR"/err | \
        "$bindir"/thot_get_nbest_for_trg -n ${n_val} -p -T "$TMP" 2>> "$SDIR"/err > "$outfile" ; ${PIPE_FAIL} || \
        { echo "Error while executing filter_ttable" >> "$SDIR"/log ; return 1 ; }

    # Write date to log file
    echo "Processing of file ${ttable_file} finished ("`date`")" >> "$SDIR"/log 

    echo "" > "$SDIR"/filter_ttable_end
}

#############
gen_log_err_files()
{
    # Copy log file to its final location
    if [ -f "$SDIR"/log ]; then
        cp "$SDIR"/log "${outfile}".filter_log
    fi

    # Generate file for error diagnosing
    if [ -f "$SDIR"/err ]; then
        cp "$SDIR"/err "${outfile}".filter_err
    fi
}

#############
report_errors()
{
    # Check errors
    num_err=`$GREP "Error while executing thot_filter_ttable_given_corpus" ${outfile}.filter_log | wc -l`
    if [ ${num_err} -gt 0 ]; then
        echo "Error during the execution of thot_pbs_filter_ttable (thot_filter_ttable_given_corpus)" >&2
        if [ -f "${outfile}".filter_err ]; then
            echo "File ${outfile}.filter_err contains information for error diagnosing" >&2
        fi
    else
        echo "Synchronization error" >&2
        if [ -f "${outfile}".filter_err ]; then
            echo "File ${outfile}.filter_err contains information for error diagnosing" >&2
        fi
    fi
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
        if [ ! -f "${ttable_file}" ]; then
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
        if [ ! -f "${test_corpus_file}" ]; then
            echo "Error: file ${test_corpus_file} does not exist!" >&2
            exit 1
        fi
    fi

    # create TMP directory
    TMP="${tmpdir}/thot_pbs_filter_ttable_tmp_$$"
    mkdir "$TMP" || { echo "Error: temporary directory cannot be created" >&2 ; exit 1; }

    # create shared directory
    SDIR="${sdir}/thot_pbs_filter_ttable_sdir_$$"
    mkdir "$SDIR" || { echo "Error: shared directory cannot be created" >&2 ; exit 1; }
    
    # remove temp directories on exit
    if [ $debug -eq 0 ]; then
        trap 'rm -rf "$TMP" "$SDIR" 2>/dev/null' EXIT
    fi

    # create log file
    echo "*** Parallel process started at: " `date` > "$SDIR"/log
    echo "">> "$SDIR"/log

    # process the input

    # filter table
    create_script "$SDIR"/filter_ttable filter_ttable || exit 1
    launch "$SDIR"/filter_ttable job_id || exit 1
    
    ### Check that all queued jobs are finished
    sync "$SDIR"/filter_ttable "${job_id}" || { gen_log_err_files ; report_errors ; exit 1; }

    # Add footer to log file
    echo "">> "$SDIR"/log
    echo "*** Parallel process finished at: " `date` >> "$SDIR"/log

    # Generate log and err files
    gen_log_err_files
fi
