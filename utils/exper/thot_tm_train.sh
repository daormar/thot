# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Trains a phrase-based model given a parallel corpus.

# \textbf{Categ}: modelling

########
print_desc()
{
    echo "thot_tm_train written by Daniel Ortiz"
    echo "thot_tm_train trains a translation model from a bilingual corpus"
    echo "type \"thot_tm_train --help\" to get usage information"
}

########
version()
{
    echo "thot_tm_train is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_tm_train           [-pr <int>]"
    echo "                        -s <string> -t <string> -o <string>"
    echo "                        [-n <int>] [-np <float>]"
    echo "                        [-af <float>]"
    echo "                        [-ao <string>] [-m <int>]"
    echo "                        [-unk] [-qs <string>] [-tdir <string>]"
    echo "                        [-sdir <string>] [-debug] [--help] [--version]"
    echo ""
    echo "-pr <int>               Number of processors (1 by default)"
    echo "-s <string>             File with source sentences"
    echo "-t <string>             File with target sentences"
    echo "-o <string>             Output directory common to all processors."
    echo "-n <int>                Number of iterations of the EM algorithm executed by"
    echo "                        the thot_gen_sw_model tool (5 by default)"
    echo "-af <float>             Alignment smoothing interpolation factor for"
    echo "                        single-word models"
    echo "-np <float>             Probability assigned to the alignment with the NULL"
    echo "                        word for single-word models"
    echo "-m <int>                Maximum target phrase length during phrase model"
    echo "                        estimation (7 by default)"
    echo "-ao <string>            Operation between alignments to be executed"
    echo "                        (and|or|sum|sym1|sym2|grd)."
    echo "-unk                    Introduce special unknown word symbol during"
    echo "                        estimation"
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
    echo "                            of 10 hours." 
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
    echo "--help                  Display this help and exit."
    echo "--version               Output version information and exit."
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
        echo $PWD/$file
    fi
}

########
create_desc_file()
{
    echo "thot tm descriptor ; tool: thot_tm_train" > ${outd}/tm_desc
    echo "$prefix main # source file: ${scorpus} ; target file: ${tcorpus}" >> ${outd}/tm_desc
}

########
if [ $# -lt 1 ]; then
    print_desc
    exit 1
fi

# Read parameters
pr_given=0
pr_val=1
s_given=0
t_given=0
o_given=0
n_given=0
niters=5
af_given=0
np_given=0
ao_given=0
ao_opt="-ao sym1"
m_val=7
qs_given=0
unk_given=0
tdir_given=0
tdir="/tmp"
sdir_given=0
sdir=$HOME
debug=0

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
                pr_val=$1
                pr_given=1
            fi
            ;;
        "-s") shift
            if [ $# -ne 0 ]; then
                scorpus=$1
                s_given=1
            fi
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                tcorpus=$1
                t_given=1
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                outd=$1
                o_given=1
            fi
            ;;
        "-n") shift
            if [ $# -ne 0 ]; then
                niters=$1
                n_given=1
            fi
            ;;
        "-af") shift
            if [ $# -ne 0 ]; then
                af_opt="-af $1"
                af_given=1
            else
                af_given=0
            fi
            ;;
        "-np") shift
            if [ $# -ne 0 ]; then
                np_opt="-np $1"
                np_given=1
            else
                np_given=0
            fi
            ;;
        "-m") shift
            if [ $# -ne 0 ]; then
                m_val=$1
                m_given=1
            fi
            ;;
        "-ao") shift
            if [ $# -ne 0 ]; then
                ao_opt="-ao $1"
                ao_given=1
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
        "-unk") unk_given=1
            unk_opt="-unk"
            ;;
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir="-T $1"
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
if [ ${s_given} -eq 0 ]; then
    echo "Error! -s parameter not given" >&2
    exit 1
else
    if [ ! -f ${scorpus} ]; then
        echo "Error! file ${scorpus} does not exist" >&2
        exit 1
    else
        # Obtain absolute path
        scorpus=`get_absolute_path $scorpus`
    fi
fi

if [ ${t_given} -eq 0 ]; then        
    echo "Error! -t parameter not given" >&2
    exit 1
else
    if [ ! -f ${tcorpus} ]; then
        echo "Error! file ${tcorpus} does not exist" >&2
        exit 1
    else
        # Obtain absolute path
        tcorpus=`get_absolute_path $tcorpus`
    fi
fi

if [ ${o_given} -eq 0 ]; then
    echo "Error! -o parameter not given!" >&2
    exit 1
else
    if [ -d ${outd}/main ]; then
        echo "Warning! output directory does exist" >&2 
        # echo "Error! output directory should not exist" >&2 
        # exit 1
    else
        # Create directory
        mkdir -p ${outd}/main || { echo "Error! cannot create output directory" >&2; exit 1; }
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

# Train model
prefix=$outd/main/src_trg
${bindir}/thot_pbs_gen_batch_phr_model -pr ${pr_val} \
    -s $tcorpus -t $scorpus -o $prefix -n $niters ${af_opt} ${np_opt} \
    -m ${m_val} ${ao_opt} ${unk_opt}  ${qs_opt} "${qs_par}" \
    -T $tdir -sdir $sdir ${debug_opt} || exit 1

# Create descriptor file
create_desc_file $outd
