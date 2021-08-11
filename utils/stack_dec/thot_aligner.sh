# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# thot_aligner for batch systems

# INCLUDE BASH LIBRARIES
. "${bindir}"/thot_general_lib || exit 1
. "${bindir}"/thot_simple_sched_lib || exit 1

print_desc()
{
    echo "thot_aligner written by Daniel Ortiz"
    echo "thot_aligner implements a parallel phrase-based aligner"
    echo "type \"thot_aligner --help\" to get usage information."
}

version()
{
    echo "thot_aligner is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

usage()
{
    echo "thot_aligner    [-pr <int>] [-c <string>] [-tm <string>] [-lm <string>]"
    echo "                -t <string> -r <string> -o <string>"
    echo "                [-W <float>] [-S <int>] [-A <int>] [-E <int>] [-nomon <int>]"
    echo "                [-h <int>] [-tmw <float> ... <float>]"
    echo "                [-wg <string> [-wgp <float>] ]"
    echo "                [-sdir <string>] [-qs <string>] [-v|-v1|-v2]"
    echo "                [-debug] [--help] [--version]"
    echo ""
    echo " -pr <int>         : Number of processors."
    echo " -c <string>       : Configuration file (command-line options override"
    echo "                     configuration file options)."
    echo " -tm <string>      : Prefix of phrase model files or model descriptor."
    echo "                     NOTES:"
    echo "                      a) give absolute paths when using pbs clusters."
    echo "                      b) ensure that the given path is reachable by all nodes."
    echo " -lm <string>      : Language model file name or model descriptor."
    echo " -t <string>       : File with the sentences to align."
    echo " -r <string>       : File with the reference sentences."
    echo " -o <string>       : Set output files prefix name."
    echo " -W <float>        : Maximum number of translation options to be considered"
    echo "                     per each source phrase (10 by default)."
    echo " -S <int>          : Maximum number of hypotheses that can be stored in each"
    echo "                     stack (10 by default)."
    echo " -A <int>          : Maximum length in words of the source phrases to be"
    echo "                     aligned (10 by default)."
    echo " -E <int>          : Constrain the target phrase length to be in the interval"
    echo "                     [splen-<int> , splen+<int>] where splen is the length"
    echo "                     of the source phrase to be aligned (5 by default)."
    echo " -nomon <int>      : Perform a non-monotonic search, allowing the decoder"
    echo "                     to skip up to <int> words from the last aligned source"
    echo "                     words. If <int> is equal to zero, then a monotonic"
    echo "                     search is performed (0 is the default value)."
    echo " -h <int>          : Heuristic function used: 0->None, 4->LOCAL_T,"
    echo "                     5->LOCAL_TL, 6->LOCAL_TD (0 by default)."
    echo " -tmw <float>...<float>:"
    echo "                     Set model weights, the number of weights and their"
    echo "                     meaning depends on the model type (see -m parameter)."
    echo " -wg <string>      : Print word graph after each translation, the prefix"
    echo "                     of the files is given as parameter."
    echo " -wgp <float>      : Prune word-graph using the given threshold."
    echo "                     Threshold=0 -> no pruning is performed."
    echo "                     Threshold=1 -> only the best arc arriving to each"
    echo "                                    state is retained."
    echo "                     If not given, the number of arcs is not"
    echo "                     restricted."
    echo " -sdir <string>    : Absolute path of a directory common to all"
    echo "                     processors. If not given \$HOME is used"
    echo " -qs <string>      : Specific options to be given to the qsub command"
    echo "                     (example: -qs \"-l pmem=1gb\")."
    echo " -v|-v1|-v2        : Verbose modes."     
    echo " -debug            : After ending, do not delete temporary files"
    echo "                     (for debugging purposes)"
    echo " --help            : Display this help and exit"
    echo " --version         : Output version information and exit"
}

check_process_safety()
{
    if [ ${c_given} -eq 1 ]; then
        check_process_safety_given_cfgfile $cfgfile
    else
        tmpcfgfile=`$MKTEMP`
        "${bindir}"/thot $lm $tm > ${tmpcfgfile}
        check_process_safety_given_cfgfile ${tmpcfgfile}
        rm ${tmpcfgfile}
    fi
}

check_process_safety_given_cfgfile()
{
    _cfgfile=$1
    nlines=`"${bindir}"/thot_server -c ${_cfgfile} -t 2>&1 | "$GREP" 'not process-safe' | "$WC" -l | "$AWK" '{print $1}'`
    if [ $nlines -eq 0 ]; then
        echo 'yes'
    else
        echo 'no'
    fi
}

str_is_option()
{
    echo "" | ${AWK} -v s=$1 '{if(!match(s,"-[a-zA-Z]")) print "0"; else print "1"}' 
}

alig_frag()
{
    # Write date to log file
    echo "** Processing chunk ${fragm} (started at "`date`")..." >> "$SDIR"/log
    echo "** Processing chunk ${fragm} (started at "`date`")..." > "$SDIR"/qs_alig_${fragm}.log

    "${bindir}"/thot_ms_alig ${cfg_opt} "$cfgfile" -t "$SDIR"/${src_fragm} -r "$SDIR"/${ref_fragm} ${tm_opt} "${tm}" ${lm_opt} "${lm}" \
        ${alig_pars} ${wgp_par} -wg "$SDIR"/wg_${fragm} 2>> "$SDIR"/qs_alig_${fragm}.log >"$SDIR"/qs_alig_${fragm}.out || \
        { echo "Error while executing alig_frag for $SDIR/${fragm}" >> "$SDIR"/qs_alig_${fragm}.log; return 1 ; }

    # Write date to log file
    echo "Processing of chunk ${fragm} finished ("`date`")" >> "$SDIR"/log 

    # Create sync file
    echo "" >"$SDIR"/qs_alig_${fragm}_end
}

move_wgs()
{
    incr=0
    for fragfile in `ls "$SDIR"/frag\_*`; do
        fragm=`${BASENAME} $fragfile`
        numfiles=0
        for f in `ls "$SDIR"/wg_${fragm}\_*.wg`; do
            pref=${f%.wg}
            num=${pref#$SDIR/wg_${fragm}\_}
            num=`expr $num + $incr`
            num=`echo $num | $AWK '{printf"%06d",$1}'`
            mv $f ${wgpref}_${num}.wg
            mv $pref.idx ${wgpref}_${num}.idx
            numfiles=`expr $numfiles + 1`
        done
        incr=`expr $incr + $numfiles`
    done
}

merge()
{
    # Write date to log file
    echo "** Merging translations (started at "`date`")..." >> "$SDIR"/log
    echo "** Merging translations (started at "`date`")..." > "$SDIR"/merge.log

    # merge trans files
    cat "$SDIR"/qs_alig_*.out > ${output} 2>> "$SDIR"/merge.log || \
        { echo "Error while executing merge" >> "$SDIR"/log; return 1 ; }

    echo "" > "$SDIR"/merge_end
}

gen_log_err_files()
{
    # Copy log file to its final location
    if [ -f "$SDIR"/log ]; then
        cp "$SDIR"/log ${output}.alig_log
    fi

    # Generate file for error diagnosing
    if [ -f ${output}.alig_err ]; then
        rm ${output}.alig_err
    fi
    for f in "$SDIR"/qs_alig_*.log; do
        cat $f >> ${output}.alig_err
    done
    for f in "$SDIR"/merge.log; do
        cat $f >> ${output}.alig_err
    done
}

report_errors()
{
    num_err=`$GREP "Error while executing" ${output}.alig_log | wc -l`
    if [ ${num_err} -gt 0 ]; then
        prog=`$GREP "Error while executing" ${output}.alig_log | head -1 | $AWK '{printf"%s",$4}'`
        echo "Error during the execution of thot_aligner (${prog})" >&2
        echo "File ${output}.alig_err contains information for error diagnosing" >&2
    else
        echo "Synchronization error" >&2
        echo "File ${output}.alig_err contains information for error diagnosing" >&2
    fi
}

# main
pr_given=0
num_procs=1
c_given=0
tm_given=0
lm_given=0
sents_given=0
refs_given=0
sdir=$HOME
alig_pars=""
debug=""
o_given=0
wg_given=0
qs_given=0

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
                num_procs=$1
                pr_given=1
            fi
            ;;
        "-c") shift
            if [ $# -ne 0 ]; then
                cfgfile=$1
                c_given=1
                cfg_opt="-c"
            fi
            ;;
        "-sdir") shift
            if [ $# -ne 0 ]; then
                sdir=$1
            fi
            ;;
        "-tm") shift
            if [ $# -ne 0 ]; then
                tm=$1
                tm_given=1
                tm_opt="-tm"
            fi
            ;;
        "-lm") shift
            if [ $# -ne 0 ]; then
                lm=$1
                lm_given=1
                lm_opt="-lm"
            fi
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                sents=$1
                sents_given=1
            fi
            ;;
        "-r") shift
            if [ $# -ne 0 ]; then
                refs=$1
                refs_given=1
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                output=$1
                o_given=1
            fi
            ;;
        "-W") shift
            if [ $# -ne 0 ]; then
                W=$1
                alig_pars="${alig_pars} -W $W"
            fi
            ;;
        "-S") shift
            if [ $# -ne 0 ]; then
                S=$1
                alig_pars="${alig_pars} -S $S"
            fi
            ;;
        "-A") shift
            if [ $# -ne 0 ]; then
                A=$1
                alig_pars="${alig_pars} -A $A"
            fi
            ;;
        "-E") shift
            if [ $# -ne 0 ]; then
                E=$1
                alig_pars="${alig_pars} -E $E"
            fi
            ;;
        "-nomon") shift
            if [ $# -ne 0 ]; then
                U=$1
                alig_pars="${alig_pars} -nomon $U"
            fi
            ;;
        "-h") shift
            if [ $# -ne 0 ]; then
                h=$1
                alig_pars="${alig_pars} -h $h"
            fi
            ;;
        "-tmw") shift
            if [ $# -ne 0 -a ! "`str_is_option $1`" = "1" ]; then
                end=0
                while [ $end -eq 0 ]; do
                    weights=$weights" "$1
                    if [ $# -le 1 ]; then
                        end=1
                    else
                        if [ "`str_is_option $2`" = "1" ]; then
                            end=1
                        else
                            shift
                        fi
                    fi
                done            
                alig_pars="${alig_pars} -tmw ${weights}"
            fi
            ;;
        "-wg") shift
            if [ $# -ne 0 ]; then
                wgpref=$1
                wg_given=1
            fi
            ;;
        "-wgp") shift
            if [ $# -ne 0 ]; then
                wgp=$1
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
        "-v") alig_pars="${alig_pars} -v"
            ;;
        "-v1") alig_pars="${alig_pars} -v1"
            ;;
        "-v2") alig_pars="${alig_pars} -v2"
            ;;
    esac
    shift
done

# verify parameters

if [ ${c_given} -eq 1 ]; then
    if [ ! -f ${cfgfile} ]; then
        echo "Error: configuration file does not exist" >&2
        exit 1
    fi
fi

if [ ${sents_given} -eq 0 ]; then
    echo "Error: file with sentences not given" >&2
    exit 1
else
    if [ ! -f  "${sents}" ]; then
        echo "Error: file ${sents} with sentences to be translated does not exist" >&2
        exit 1
    fi
fi

if [ ${refs_given} -eq 0 ]; then
    echo "Error: file with sentences not given" >&2
    exit 1
else
    if [ ! -f  "${refs}" ]; then
        echo "Error: file ${refs} with reference sentences does not exist" >&2
        exit 1
    fi
fi

if [ ${o_given} -eq 0 ];then
    # invalid parameters 
    echo "Error: output files prefix must be given" >&2
    exit 1
fi

# Check that test and ref files are parallel
nl_test=`wc -l $sents | $AWK '{printf"%d",$1}'`
nl_ref=`wc -l $refs | $AWK '{printf"%d",$1}'`

if [ ${nl_test} -ne ${nl_ref} ]; then
    echo "Error! test and reference files have not the same number of lines" >&2 
    exit 1
fi

process_safety=`check_process_safety`
if [ ${process_safety} = "no" ]; then
    if [ ${num_procs} -gt 1 ]; then
        echo "Warning: only one processor will be used since there are non-process-safe modules" >&2
        num_procs=1
    fi
fi

# parameters are ok

# create shared directory

if [ ! -d ${sdir} ]; then
    echo "Error: shared directory does not exist" >&2
    return 1;
fi

SDIR=`${MKTEMP} -d ${sdir}/thot_aligner_XXXXXX`

# remove temp directories on exit
if [ "$debug" != "-debug" ]; then
    trap "rm -rf "$SDIR" 2>/dev/null" EXIT
fi

# set wgp_par variable
if [ ${wg_given} -eq 1 ]; then
    if [ -z "${wgp}" ]; then
        wgp_par=""
    else
        wgp_par="-wgp ${wgp}"
    fi
else
    wgp_par=""
fi

# create log file
echo "Input file: ${sents}"> "$SDIR"/log
echo "">> "$SDIR"/log

echo "*** Parallel process started at: " `date` >> "$SDIR"/log
echo "">> "$SDIR"/log

# process input

# fragment input
input_size=`wc ${sents} 2>/dev/null | ${AWK} '{printf"%d",$(1)}'`
if [ ${input_size} -eq 0 ]; then
    echo "Error: input file ${sents} is empty" >&2
    exit 1
fi

if [ ${input_size} -lt ${num_procs} ]; then
    echo "Error: problem too small" >&2
    exit 1
fi
frag_size=`expr ${input_size} / ${num_procs}`
frag_size=`expr ${frag_size}`
nlines=${frag_size}
${SPLIT} -l ${nlines} $sents "$SDIR/src_frag_" || exit 1
${SPLIT} -l ${nlines} $refs "$SDIR/ref_frag_" || exit 1

# parallel test corpus translation for each fragment
qs_alig=""
jids=""
i=1

for f in `ls "$SDIR"/src\_frag\_*`; do
    fragm=`${BASENAME} $f`
    fragm=${fragm:4}
    src_fragm="src_"${fragm}
    ref_fragm="ref_"${fragm}

    # Obtain translations for the current fragment
    create_script "$SDIR"/qs_alig_${fragm} alig_frag || exit 1
    launch "$SDIR"/qs_alig_${fragm} job_id || exit 1
    qs_alig="${qs_alig} $SDIR/qs_alig_${fragm}"
    jids="${jids} ${job_id}"

    i=`expr $i + 1`
done

### Check that all queued jobs are finished
sync "${qs_alig}" "${jids}" || { gen_log_err_files ; report_errors ; exit 1; }

# move word graph files if generated
if [ ${wg_given} -eq 1 ]; then
    move_wgs
fi

# merge files
create_script "$SDIR"/merge merge || exit 1
launch "$SDIR"/merge job_id || exit 1

### Check that all queued jobs are finished
sync "$SDIR"/merge "${job_id}" || { gen_log_err_files ; report_errors ; exit 1; }

# Add footer to log file
echo "">> "$SDIR"/log
echo "*** Parallel process finished at: " `date` >> "$SDIR"/log

# Generate log and err files
gen_log_err_files
