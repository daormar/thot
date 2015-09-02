# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# thot_decoder for batch systems

print_desc()
{
    echo "thot_decoder written by Daniel Ortiz"
    echo "thot_decoder implements a parallel decoder"
    echo "type \"thot_decoder --help\" to get usage information."
}

version()
{
    echo "thot_decoder is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

config()
{
    ${bindir}/thot_ms_dec --config
}

usage()
{
    echo "thot_decoder    [-pr <int>] [-c <string>]"
    echo "                [-tm <string>] [-lm <string>] -t <string>"
    echo "                -o <string> [-W <float>] [-S <int>]"
    echo "                [-A <int>] [-U <int>] [-I <int>] [-be] [-G <int>]"
    echo "                [-h <int>] [ -mon] [-tmw <float> ... <float>]"
    echo "                [-wg <string> [-wgp <float>] ]"
    echo "                [-sdir <string>] [-qs <string>] [-v|-v1|-v2]"
    echo "                [-debug] [--help] [--version] [--config]"
    echo ""
    echo " -pr <int>         : Number of processors."
    echo " -c <string>       : Configuration file (command-line options override"
    echo "                     configuration file options)."
    echo " -tm <string>      : Prefix of the phrase model files."
    echo "                     NOTES:"
    echo "                      a) give absolute paths when using pbs clusters."
    echo "                      b) ensure that the given path is reachable by all nodes."
    echo " -lm <string>      : Language model file name."
    echo " -t <string>       : File with the sentences to translate."
    echo " -o <string>       : Set output files prefix name."
    echo " -W <float>        : Maximum number of inverse translations/Threshold"
    echo "                     (10 by default)."
    echo " -S <int>          : S parameter (1024 or 64 by default)."
    echo " -A <int>          : A parameter (10 by default)."
    echo " -U <int>          : Maximum number of jumped words (unrestricted by."
    echo "                     default)."
    echo " -I <int>          : Number of hypotheses expanded at each iteration."
    echo "                     (1 by default)."
    echo " -be               : Execute a best-first algorithm."
    echo " -G <int>          : Granularity parameter (0 by default, may not"
    echo "                     be available depending on the compiler options)."
    echo " -h <int>          : Heuristic function used: 0->None, 4->LOCAL_T,"
    echo "                     5->LOCAL_TL, 6->LOCAL_TD (0 by default)."
    echo " -mon              : Perform a monotone search."
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
    echo "                     restricted.";
    echo " -sdir <string>    : Absolute path of a directory common to all"
    echo "                     processors. If not given \$HOME is used"
    echo " -qs <string>      : Specific options to be given to the qsub command"
    echo "                     (example: -qs \"-l pmem=1gb\")."
    echo " -v|-v1|-v2        : Verbose modes."     
    echo " -debug            : After ending, do not delete temporary files"
    echo "                     (for debugging purposes)"
    echo " --help            : Display this help and exit"
    echo " --version         : Output version information and exit"
    echo " --config          : Show current configuration."
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

str_is_option()
{
    echo "" | ${AWK} -v s=$1 '{if(!match(s,"-[a-zA-Z]")) print "0"; else print "1"}' 
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

trans_frag()
{
    ${bindir}/thot_ms_dec ${cfg_opt} -t $SDIR/${fragm} ${dec_pars} \
        ${wg_par}wg_${fragm} >$SDIR/qs_trans_${fragm}.out 2>$SDIR/qs_trans_${fragm}.log || \
        { echo "Error while executing thot_ms_dec for $SDIR/${fragm}" >> $SDIR/qs_trans_${fragm}.log; }

    echo "" >$SDIR/qs_trans_${fragm}_end
}

move_wgs()
{
    incr=0
    for fragfile in `ls $SDIR/frag\_*`; do
        fragm=`${BASENAME} $fragfile`
        numfiles=0
        for f in `ls $SDIR/wg_${fragm}\_*.wg`; do
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
    # merge trans files
    cat $SDIR/qs_trans_*.out > ${output}

    echo "" > $SDIR/merge_end
}

pr_given=0
num_procs=1
c_given=0
tm_given=0
lm_given=0
sents_given=0
sdir=$HOME
dec_pars=""
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
        "--config") config
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
                cfg_opt="-c $cfgfile"
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
                dec_pars="${dec_pars} -tm $tm"
            fi
            ;;
        "-lm") shift
            if [ $# -ne 0 ]; then
                lm=$1
                lm_given=1
                dec_pars="${dec_pars} -lm $lm"
            fi
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                sents=$1
                sents_given=1
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                output=$1
                o_given=1
            fi
            ;;
        "-b") shift
            if [ $# -ne 0 ]; then
                b=$1
                dec_pars="${dec_pars} -b $b"
            fi
            ;;
        "-W") shift
            if [ $# -ne 0 ]; then
                W=$1
                dec_pars="${dec_pars} -W $W"
            fi
            ;;
        "-S") shift
            if [ $# -ne 0 ]; then
                S=$1
                dec_pars="${dec_pars} -S $S"
            fi
            ;;
        "-A") shift
            if [ $# -ne 0 ]; then
                A=$1
                dec_pars="${dec_pars} -A $A"
            fi
            ;;
        "-U") shift
            if [ $# -ne 0 ]; then
                U=$1
                dec_pars="${dec_pars} -U $U"
            fi
            ;;
        "-I") shift
            if [ $# -ne 0 ]; then
                I=$1
                dec_pars="${dec_pars} -I $I"
            fi
            ;;
        "-be") dec_pars="${dec_pars} -be"
            ;;
        "-G") shift
            if [ $# -ne 0 ]; then
                G=$1
                dec_pars="${dec_pars} -G $G"
            fi
            ;;
        "-h") shift
            if [ $# -ne 0 ]; then
                h=$1
                dec_pars="${dec_pars} -h $h"
            fi
            ;;
        "-mon") dec_pars="${dec_pars} -mon"
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
                dec_pars="${dec_pars} -tmw ${weights}"
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
        "-v") dec_pars="${dec_pars} -v"
            ;;
        "-v1") dec_pars="${dec_pars} -v1"
            ;;
        "-v2") dec_pars="${dec_pars} -v2"
            ;;
    esac
    shift
done

# verify parameters

if [ ${sents_given} -eq 0 ]; then
    echo "Error: file with sentences not given" >&2
    exit 1
else
    if [ ! -f  "${sents}" ]; then
        echo "Error: file ${sents} with sentences does not exist" >&2
        exit 1
    fi
fi

if [ ${o_given} -eq 0 ];then
    # invalid parameters 
    echo "Error: output files prefix must be given" >&2
    exit 1
fi

# parameters are ok

# create shared directory

if [ ! -d ${sdir} ]; then
    echo "Error: shared directory does not exist" >&2
    return 1;
fi

SDIR=`${MKTEMP} -d ${sdir}/thot_decoder_XXXXXX`

#### OLD CODE (NOT SAFE WHEN DIRECTORIES CREATED BY OTHER INSTANCES
####           OF THIS SCRIPT ARE NOT REMOVED)
# SDIR="${sdir}/thot_decoder_$$"
# mkdir $SDIR || { echo "Error: shared directory cannot be created" ; exit 1; }

# remove temp directories on exit
if [ "$debug" != "-debug" ]; then
    trap "rm -rf $SDIR 2>/dev/null" EXIT
fi

#
if [ ${wg_given} -eq 1 ]; then
    if [ -z "${wgp}" ]; then
        wg_par="-wg $SDIR/"
    else
        wg_par="-wgp ${wgp} -wg $SDIR/"
    fi
else
    wg_par=""
fi

# create log file
echo "*** Parallel process started at: " `date` > ${output}.log
echo "">> ${output}.log

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
frag_size=`expr ${frag_size} + 1`
nlines=${frag_size}
${SPLIT} -l ${nlines} $sents $SDIR/frag\_ || exit 1

# parallel test corpus translation for each fragment
qs_trans=""
i=1

for f in `ls $SDIR/frag\_*`; do
    fragm=`${BASENAME} $f`
    # Obtain translations for the current fragment
    create_script $SDIR/qs_trans_${fragm} trans_frag
    launch $SDIR/qs_trans_${fragm}
    qs_trans="${qs_trans} $SDIR/qs_trans_${fragm}"

    i=`expr $i + 1`
done

### Check that all queued jobs are finished
sync "${qs_trans}"

# move word graph files if generated
if [ ${wg_given} -eq 1 ]; then
    move_wgs
fi

# merge files
create_script $SDIR/merge merge
launch $SDIR/merge

# merge log files
cat $SDIR/qs_trans_*.log >> ${output}.log

### Check that all queued jobs are finished
sync $SDIR/merge

# Add footer to log file
echo "">> ${output}.log
echo "*** Parallel process finished at: " `date` >> ${output}.log

# Check errors
num_err=`$GREP "Error while executing thot_ms_dec" ${output}.log | wc -l`
if [ ${num_err} -gt 0 ]; then
    echo "Error during the execution of thot_decoder (thot_dec_ms)" >&2
    exit 1
fi
