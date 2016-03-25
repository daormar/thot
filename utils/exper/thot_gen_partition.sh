# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Generates a corpus partition given a text file.

########
print_desc()
{
    echo "thot_gen_partition written by Daniel Ortiz"
    echo "thot_gen_partition generates a corpus partition given a text file"
    echo "type \"thot_gen_partition --help\" to get usage information"
}

########
version()
{
    echo "thot_gen_partition is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_gen_partition      -s <string> t <string> -e <int> -n <int>"
    echo "                        [--help] [--version]"
    echo ""
    echo "-s <string>             File with source sentences"
    echo "-t <string>             File with target sentences"
    echo "-e <int>                Number of sentences to exclude from training"
    echo "-n <int>                Size of test and development corpora"
    echo "--help                  Display this help and exit"
    echo "--version               Output version information and exit"
}

########
if [ $# -lt 1 ]; then
    print_desc
    exit 1
fi

# Read parameters
s_given=0
t_given=0
e_given=0
n_given=0

while [ $# -ne 0 ]; do
    case $1 in
        "--help") usage
            exit 0
            ;;
        "--version") version
            exit 0
            ;;
        "-s") shift
            if [ $# -ne 0 ]; then
                srcfile=$1
                s_given=1
            fi
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                trgfile=$1
                t_given=1
            fi
            ;;
        "-e") shift
            if [ $# -ne 0 ]; then
                esize=$1
                e_given=1
            fi
            ;;
        "-n") shift
            if [ $# -ne 0 ]; then
                dtsize=$1
                n_given=1
            fi
            ;;
    esac
    shift
done

# Check parameters
if [ ${s_given} -eq 0 ]; then        
    echo "Error! -s parameter not given" >&2
    exit 1
else
    if [ ! -f ${srcfile} ]; then
        echo "Error! file ${srcfile} does not exist" >&2
        exit 1
    fi
fi

if [ ${t_given} -eq 0 ]; then        
    echo "Error! -t parameter not given" >&2
    exit 1
else
    if [ ! -f ${trgfile} ]; then
        echo "Error! file ${trgfile} does not exist" >&2
        exit 1
    fi
fi

if [ ${e_given} -eq 0 ]; then
    echo "Error! -e parameter not given" >&2
    exit 1
fi

if [ ${n_given} -eq 0 ]; then
    echo "Error! -n parameter not given" >&2
    exit 1
fi

if [ `expr 2 \* ${dtsize}` -lt ${e_given} ]; then
    echo "Error! 2 times -n option should be lower than the value of -e" >&2
    exit 1    
fi

# Obtain number of lines for files
nlsrc=`wc -l $srcfile | $AWK '{printf"%s",$1}'`
nltrg=`wc -l $trgfile | $AWK '{printf"%s",$1}'`

if [ $nlsrc -ne $nltrg ]; then
    echo "Error! source and target files have a different number of lines" >&2
    exit 1
fi

nl=$nlsrc

if [ $nl -lt $esize ]; then
    echo "Error! value of -e is greater than the number of available sentences" >&2
    exit 1
fi

## Process parameters

# Shuffle corpus
${bindir}/thot_shuffle 31415 ${srcfile} > ${srcfile}.shuff
${bindir}/thot_shuffle 31415 ${trgfile} > ${trgfile}.shuff

# Generate training and excluded sentences files
trsize=`expr $nl - $esize`
$HEAD -$trsize ${srcfile}.shuff > ${srcfile}.train
$HEAD -$trsize ${trgfile}.shuff > ${trgfile}.train
$TAIL -$esize ${srcfile}.shuff > ${srcfile}.excluded
$TAIL -$esize ${trgfile}.shuff > ${trgfile}.excluded

# Generate dev partition
$HEAD -$dtsize ${srcfile}.excluded > ${srcfile}.dev
$HEAD -$dtsize ${trgfile}.excluded > ${trgfile}.dev

# Generate test partition
tmp=`expr 2 \* $dtsize`
$HEAD -$tmp ${srcfile}.excluded | $TAIL -$dtsize > ${srcfile}.test
$HEAD -$tmp ${trgfile}.excluded | $TAIL -$dtsize > ${trgfile}.test
