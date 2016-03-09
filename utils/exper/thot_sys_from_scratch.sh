# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Automatically obtains statistical machine translation results.

########
print_desc()
{
    echo "thot_sys_from_scratch written by Daniel Ortiz"
    echo "thot_sys_from_scratch generates a translation system from scratch"
    echo "type \"thot_sys_from_scratch --help\" to get usage information"
}

########
version()
{
    echo "thot_sys_from_scratch is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_sys_from_scratch   -o <string>"
    echo "                        [-nit <int>] [-n <int>] [-tqm <string>]"
    # echo "                        [-nit <int>] [-n <int>] [-tqm <string>]"
    echo "                        [-qs <string>] [-tdir <string>]"
    echo "                        [-sdir <string>] [-debug] [--help] [--version]"
    echo ""
    echo "-o <string>             Output directory common to all processors"
    echo "-nit <int>              Number of iterations of the EM algorithm when training"
    echo "                        single word models (5 by default)"
    echo "-n <int>                Order of the n-gram language models (4 by default)"
    # echo "-tqm <string>           Set translation quality measure for tuning"
    # echo "                        (BLEU by default, other options: WER)"
    echo "-qs <string>            Specific options to be given to the qsub"
    echo "                        command (example: -qs \"-l pmem=1gb\")"
    echo "                        NOTES:"
    echo "                         a) ignore this if not using a PBS cluster"
    echo "                         b) -qs option may be crucial to ensure the correct"
    echo "                            execution of the tool. The main purpose of -qs"
    echo "                            is to reserve the required cluster resources."
    echo "                            If the necessary resources are not met the"
    echo "                            execution will abort."
    echo "                            Resources are reserved in different ways depending"
    echo "                            on the cluster software. For instance, if using SGE"
    echo "                            software, -qs \"-l h_vmem=1G,h_rt=10:00:00\","
    echo "                            requests 1GB of virtual memory and a time limit"
    echo "                            of 10 hours" 
    echo "-tdir <string>          Directory for temporary files (/tmp by default)."
    echo "                        NOTES:"
    echo "                         a) give absolute paths when using pbs clusters"
    echo "                         b) ensure there is enough disk space in the partition"
    echo "-sdir <string>          Absolute path of a directory common to all"
    echo "                        processors. If not given, \$HOME will be used."
    echo "                        NOTES:"
    echo "                         a) give absolute paths when using pbs clusters"
    echo "                         b) ensure there is enough disk space in the partition"
    echo "-debug                  After ending, do not delete temporary files"
    echo "                        (for debugging purposes)"
    echo "--help                  Display this help and exit"
    echo "--version               Output version information and exit"
}

########
is_absolute_path()
{
    case $1 in
        /*) echo 1 ;;
        *) echo 0 ;;
    esac
}

########
get_absolute_path()
{
    file=$1
    # Check if an absolute path was given
    absolute=`is_absolute_path $file`
    if [ $absolute -eq 1 ]; then
        echo $file
    else
        oldpwd=$PWD
        basetmp=`$BASENAME $PWD/$file`
        dirtmp=`$DIRNAME $PWD/$file`
        cd $dirtmp
        result=${PWD}/${basetmp}
        cd $oldpwd
        echo $result
    fi
}

########
if [ $# -lt 1 ]; then
    print_desc
    exit 1
fi

# Read parameters
o_given=0
nit_given=0
nitval=5
n_given=0
n_val=4
tqm="BLEU"
tqm_given=0
qs_given=0
tdir_given=0
tdir="/tmp"
sdir_given=0
sdir=$HOME
debug=0
debug_opt=""

while [ $# -ne 0 ]; do
    case $1 in
        "--help") usage
            exit 0
            ;;
        "--version") version
            exit 0
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                outd=$1
                o_given=1
            fi
            ;;
        "-nit") shift
            if [ $# -ne 0 ]; then
                nitval=$1
                nit_given=1
            fi
            ;;
        "-n") shift
            if [ $# -ne 0 ]; then
                n_val=$1
                n_given=1
            fi
            ;;
        "-tqm") shift
            if [ $# -ne 0 ]; then
                tqm=$1
                tqm_given=1
            fi
            ;;
        "-qs") shift
            if [ $# -ne 0 ]; then
                qs_opt="-qs"
                qs_par="$1"
                qs_given=1
            else
                qs_given=0
            fi
            ;;
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1
                tdir_given=1
            fi
            ;;
        "-sdir") shift
            if [ $# -ne 0 ]; then
                sdir=$1
                sdir_given=1
            fi
            ;;
        "-debug") debug=1
            debug_opt="-debug"
            ;;
    esac
    shift
done

# Check parameters

if [ ${o_given} -eq 0 ]; then
    echo "Error! -o parameter not given!" >&2
    exit 1
else
    if [ -d ${outd} ]; then
        echo "Warning! output directory does exist" >&2 
    else
        # Create directory
        mkdir -p ${outd} || { echo "Error! cannot create output directory" >&2; exit 1; }
    fi
    # Obtain absolute path
    outd=`get_absolute_path $outd`
fi

if [ ${tdir_given} -eq 1 ]; then
    if [ ! -d ${tdir} ]; then
        echo "Error! directory ${tdir} does not exist" >&2
        exit 1   
    fi         
fi

if [ ${sdir_given} -eq 1 ]; then
    if [ ! -d ${sdir} ]; then
        echo "Error! directory ${sdir} does not exist" >&2
        exit 1            
    fi
fi

## Process parameters

# Create empty corpus dir
if [ ! -d ${outd}/empty_corpus ]; then
    mkdir ${outd}/empty_corpus || exit 1
fi

# Create empty corpus
echo "<artificial_token>" > ${outd}/empty_corpus/src.train
scorpus_train=${outd}/empty_corpus/src.train

echo "<artificial_token>" > ${outd}/empty_corpus/trg.train
tcorpus_train=${outd}/empty_corpus/trg.train

# Train models
if [ -f ${scorpus_train} -a -f ${tcorpus_train} ]; then

    # Train language model
    echo "**** Training language model" >&2
    ${bindir}/thot_lm_train -c ${tcorpus_train} -o ${outd}/lm -n ${n_val} -unk \
        ${qs_opt} "${qs_par}" -tdir $tdir -sdir $sdir ${debug_opt} || exit 1

    # Train translation model
    echo "**** Training translation model" >&2
    ${bindir}/thot_tm_train -s ${scorpus_train} -t ${tcorpus_train} -o ${outd}/tm -n ${nitval} \
        ${qs_opt} "${qs_par}" -tdir $tdir -sdir $sdir ${debug_opt} || exit 1

else
    echo "Error! training files do not exist" >&2
    exit 1                
fi

# Generate cfg file
echo "**** Generating configuration file" >&2
${bindir}/thot_gen_cfg_file $outd/lm/lm_desc $outd/tm/tm_desc > $outd/sys_from_scratch.cfg || exit 1
echo "" >&2
