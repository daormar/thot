# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Carry out a set of experiments to determine sample size for smt
# evaluation

########
print_desc()
{
    echo "thot_smt_samplesize written by Daniel Ortiz"
    echo "thot_smt_samplesize allows to determine sample size for smt evaluation"
    echo "type \"thot_smt_samplesize --help\" to get usage information"
}

########
version()
{
    echo "thot_smt_samplesize is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_smt_samplesize   -r <string> -t <string> -e <int>"
    echo "                      [-tdir <string>] [--help] [--version]"
    echo ""
    echo "-r <string>             File with reference sentences."
    echo "-t <string>             File with system translations."
    echo "-e <int>                Step size in number of sentences."
    echo "-tdir <string>          Directory for temporary files (/tmp by default)."
    echo "--help                  Display this help and exit."
    echo "--version               Output version information and exit."
    echo ""
}

##################

if [ $# -eq 0 ]; then
    print_desc
    exit 1
fi

# Read parameters
r_given=0
t_given=0
e_given=0
tdir_given=0
tdir="/tmp"

while [ $# -ne 0 ]; do
    case $1 in
        "--help") usage
            exit 0
            ;;
        "--version") version
            exit 0
            ;;
        "-r") shift
            if [ $# -ne 0 ]; then
                rcorpus=$1
                r_given=1
            fi
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                tcorpus=$1
                t_given=1
            fi
            ;;
        "-e") shift
            if [ $# -ne 0 ]; then
                e_val=$1
                e_given=1
            fi
            ;;
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1
                tdir_given=1
            fi
            ;;
    esac
    shift
done

# Check parameters
if [ ${r_given} -eq 0 ]; then
    echo "Error! -r parameter not given!" >&2
    exit 1
else
    if [ ! -f ${rcorpus} ]; then
        echo "Error! file ${rcorpus} does not exist" >&2
        exit 1
    fi
fi

if [ ${t_given} -eq 0 ]; then
    echo "Error! -t parameter not given!" >&2
    exit 1
else
    if [ ! -f ${tcorpus} ]; then
        echo "Error! file ${tcorpus} does not exist" >&2
        exit 1
    fi
fi

if [ ${e_given} -eq 0 ]; then
    echo "Error! -e parameter not given!" >&2
    exit 1
fi

# Create tmp directory
if [ ${tdir_given} -eq 1 ]; then
    if [ ! -d ${tdir} ]; then
        echo "Error! directory ${tdir} does not exist" >&2
        exit 1            
    fi
fi

TDIR_SMT_SAMPLESIZE=`${MKTEMP} -d ${tdir}/thot_smt_samplesize_XXXXXX`

# Remove temp directories on exit
if [ "$debug_opt" != "-debug" ]; then
    trap "rm -rf $TDIR_SMT_SAMPLESIZE 2>/dev/null" EXIT
fi

# Shuffle corpus files
${bindir}/thot_shuffle 31415 ${TDIR_SMT_SAMPLESIZE} $rcorpus > ${TDIR_SMT_SAMPLESIZE}/rcorpus_shuffled
${bindir}/thot_shuffle 31415 ${TDIR_SMT_SAMPLESIZE} $tcorpus > ${TDIR_SMT_SAMPLESIZE}/tcorpus_shuffled

# Obtain number of corpus sentences
num_sents=`$WC -l $rcorpus | $AWK '{printf"%s",$1}'`

# Obtain 95% confidence intervals for subsets of shuffled corpus files
# of increasing size
curr_size=${e_val}
end=0
while [ $end -eq 0 ]; do
    # Check ending condition
    if [ ${curr_size} -eq $num_sents ]; then
        end=1
    fi

    # Define corpus names
    ref=${TDIR_SMT_SAMPLESIZE}/rcorpus_shuffled_${curr_size}
    sys=${TDIR_SMT_SAMPLESIZE}/tcorpus_shuffled_${curr_size}
    
    # Extract subset
    $HEAD -${curr_size} ${TDIR_SMT_SAMPLESIZE}/rcorpus_shuffled > $ref
    $HEAD -${curr_size} ${TDIR_SMT_SAMPLESIZE}/tcorpus_shuffled > $sys
    
    # Evaluate subset
    ${bindir}/thot_conf_interv_smt 31415 $ref $sys ${curr_size} 10000 BLEU > ${TDIR_SMT_SAMPLESIZE}/conf_int_${curr_size}
    
    # Print interval
    conf_int=`$GREP 'conf.int.95' ${TDIR_SMT_SAMPLESIZE}/conf_int_${curr_size}`
    echo "${curr_size} ; ${conf_int}"
    
    # Increase current size
    curr_size=`expr ${curr_size} + ${e_val}`
    if [ ${curr_size} -gt $num_sents ]; then
        curr_size=${num_sents}
    fi
done
