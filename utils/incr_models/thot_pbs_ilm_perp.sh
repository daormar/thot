# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Filters a translation table given a test corpus and an integer value
# that sets the maximum number of translation options for each target
# phrase that are considered during a translation process. The utility
# can be executed in a PBS cluster.

# INCLUDE BASH LIBRARIES
. "${bindir}"/thot_general_lib || exit 1
. "${bindir}"/thot_simple_sched_lib || exit 1

#############
exclude_readonly_vars()
{
    "${AWK}" -F "=" 'BEGIN{
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

#############
ilm_perp()
{
    # Write date to log file
    echo "** Processing file ${cfile} (started at "`date`")..." >> "$SDIR/log"
    
    "$bindir"/thot_ilm_perp -lm "${lmfile}" -c "${cfile}" -n ${n_val} ${add_opts} 2> "$SDIR/err" > "$outfile" || \
        { echo "Error while executing thot_ilm_perp" >> "$SDIR/log" ; return 1 ; }

    # Write date to log file
    echo "Processing of file ${cfile} finished ("`date`")" >> "$SDIR/log" 

    echo "" > "$SDIR/ilm_perp_end"
}

#############
gen_log_err_files()
{
    # Copy log file to its final location
    if [ -f "$SDIR/log" ]; then
        cp "$SDIR/log" "${outfile}.ilmp_log"
    fi

    # Generate file for error diagnosing
    if [ -f "$SDIR/err" ]; then
        cp "$SDIR/err" "${outfile}.ilmp_err"
    fi
}

#############
report_errors()
{
    # Check errors
    num_err=`$GREP "Error while executing thot_ilm_perp" ${outfile}.ilmp_log | wc -l`
    if [ ${num_err} -gt 0 ]; then
        echo "Error during the execution of thot_pbs_ilm_perp (thot_ilm_perp)" >&2
        if [ -f "${output}.ilmp_err" ]; then
            echo "File ${output}.ilmp_err contains information for error diagnosing" >&2
        fi
    else
        echo "Synchronization error" >&2
        if [ -f "${output}.ilmp_err" ]; then
            echo "File ${output}.ilmp_err contains information for error diagnosing" >&2
        fi
    fi
}

#############
if [ $# -eq 0 ]; then
    echo "Usage: thot_pbs_ilm_perp -c <string> -lm <string> -n <int>" >&2
    echo "                         -o <string> {-i | -jm | -cjm} [-qs <string>]" >&2
    echo "                         [-tdir <string>] [-sdir <string>]" >&2
    echo "                         [-debug] [-v|-v1]" >&2
    echo "-c <string>              Corpus file to be processed." >&2
    echo "-lm <string>             Language model file name." >&2
    echo "-n <int>                 Order of the n-grams." >&2
    echo "-o <string>              Prefix of output files." >&2
    echo "-i                       Use interpolated model." >&2
    echo "-jm                      Use Jelinek-Mercer n-gram models." >&2
    echo "-cjm                     Use cache-based Jelinek-Mercer n-grams models." >&2
    echo "-qs <string>             Specific options to be given to the qsub command"
    echo "                         (example: -qs \"-l pmem=1gb\")."
    echo "-tdir <string>           Use <string> for temporaries instead of /tmp" >&2
    echo "                         during the generation of the phrase model" >&2
    echo "-sdir <string>           Absolute path of a directory common to all" >&2
    echo "                         processors. If not given, \$HOME will be used." >&2
    echo "-debug                   After ending, do not delete temporary files" >&2
    echo "                         (for debugging purposes)" >&2
    echo "-v|-v1                   Verbose modes." >&2
else
    lm_given=0
    lmfile=""
    c_given=0
    cfile=""
    n_given=0
    o_given=0
    tmpdir="/tmp"
    sdir=$HOME
    debug=0
    add_opts=""
    while [ $# -ne 0 ]; do
        case $1 in
            "-c") shift
                if [ $# -ne 0 ]; then
                    cfile=$1
                    c_given=1
                fi
                ;;
            "-lm") shift
                if [ $# -ne 0 ]; then
                    lmfile=$1
                    lm_given=1
                fi
                ;;
            "-n") shift
                if [ $# -ne 0 ]; then
                    n_val=$1
                    n_given=1
                fi
                ;;
            "-o") shift
                if [ $# -ne 0 ]; then
                    outfile=$1
                    o_given=1
                fi
                ;;
            "-i") add_opts="${add_opts} -i"
                ;;
            "-jm") add_opts="${add_opts} -jm"
                ;;
            "-cjm") add_opts="${add_opts} -cjm"
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
                    tmpdir=$1
                else
                    tmpdir="/tmp"
                fi
                ;;
            "-sdir") shift
                if [ $# -ne 0 ]; then
                    sdir=$1                
                fi
                ;;
            "-debug") debug=1
                ;;
            "-v") add_opts="${add_opts} -v"
                ;;
            "-v1") add_opts="${add_opts} -v1"
                ;;
        esac
        shift
    done

    # verify parameters

    if [ ${c_given} -eq 0 ]; then
        echo "Error: -c option not given!" >&2
        exit 1
    else
        if [ ! -f ${cfile} ]; then
            echo "Error: file ${cfile} does not exist!" >&2
            exit 1
        fi
    fi

    if [ ${lm_given} -eq 0 ]; then
        echo "Error: -lm option not given!" >&2
        exit 1
    else
        if [ ! -f "${lmfile}" ]; then
            echo "Error: file ${lmfile} does not exist!" >&2
            exit 1
        fi
    fi

    if [ ${n_given} -eq 0 ]; then
        echo "Error: -n option not given!" >&2
        exit 1
    fi

    if [ ${o_given} -eq 0 ]; then
        echo "Error: -o option not given!" >&2
        exit 1
    fi

    # create TMP directory
    TMP="${tmpdir}/thot_pbs_ilm_perp_tmp_$$"
    mkdir "$TMP" || { echo "Error: temporary directory cannot be created" >&2 ; exit 1; }

    # create shared directory
    SDIR="${sdir}/thot_pbs_ilm_perp_sdir_$$"
    mkdir "$SDIR" || { echo "Error: shared directory cannot be created" >&2 ; exit 1; }
    
    # remove temp directories on exit
    if [ $debug -eq 0 ]; then
        trap 'rm -rf "$TMP" "$SDIR" 2>/dev/null' EXIT
    fi

    # create log file
    echo "*** Parallel process started at: " `date` > $SDIR/log
    echo "">> "$SDIR/log"

    # process the input

    # Calculate perplexity
    create_script "$SDIR/ilm_perp" ilm_perp || exit 1
    launch "$SDIR/ilm_perp" job_id || exit 1
    
    ### Check that all queued jobs are finished
    sync "$SDIR/ilm_perp" "${job_id}" || { gen_log_err_files ; report_errors ; exit 1; }

    # Add footer to log file
    echo "">> "$SDIR/log"
    echo "*** Parallel process finished at: " `date` >> "$SDIR/log"
    
    # Generate log and err files
    gen_log_err_files

fi
