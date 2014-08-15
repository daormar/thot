# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Trains a phrase-based model given a parallel corpus.

# \textbf{Categ}: modelling

########
print_desc()
{
    echo "thot_lm_train written by Daniel Ortiz"
    echo "thot_lm_train trains a language model from a monolingual corpus"
    echo "type \"thot_lm_train --help\" to get usage information"
}

########
version()
{
    echo "thot_lm_train is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_lm_train      [-pr <int>]"
    echo "                   -c <string> -o <string> -n <int> [-unk]"
    echo "                   [-qs <string>] [-tdir <string>] [-sdir <string>]"
    echo "                   [-debug] [--help] [--version]"
    echo ""
    echo "-pr <int>          Number of processors."
    echo "-c <string>        Corpus file."
    echo "-o <string>        Output directory common to all processors."
    echo "-n <int>           Order of the n-grams."
    echo "-unk               Reserve probability mass for the unknown word."
    echo "-qs <string>       Specific options to be given to the qsub command"
    echo "                   (example: -qs \"-l pmem=1gb\")."
    echo "                   NOTES:"
    echo "                    a) ignore this if not using a PBS cluster"
    echo "                    b) -qs option may be crucial to ensure the correct"
    echo "                       execution of the tool. The main purpose of -qs"
    echo "                       is to reserve the required cluster resources."
    echo "                       If the necessary resources are not met the"
    echo "                       execution will abort."
    echo "                       Resources are reserved in different ways depending"
    echo "                       on the cluster software. For instance, if using SGE"
    echo "                       software, -qs \"-l h_vmem=4G,h_rt=10:00:00\","
    echo "                       requests 4GB of virtual memory and a time limit"
    echo "                       of 10 hours." 
    echo "-tdir <string>     Directory for temporary files (/tmp by default)."
    echo "                   NOTES:"
    echo "                    a) give absolute paths when using pbs clusters."
    echo "                    b) ensure there is enough disk space in the partition."
    echo "-sdir <string>     Absolute path of a directory common to all"
    echo "                   processors. If not given, \$HOME will be used."
    echo "                   NOTES:"
    echo "                    a) give absolute paths when using pbs clusters."
    echo "                    b) ensure there is enough disk space in the partition."
    echo "-debug             After ending, do not delete temporary files"
    echo "                   (for debugging purposes)."
    echo "--help             Display this help and exit."
    echo "--version          Output version information and exit."
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
    echo "tool: thot_lm_train" > ${outd}/lm_desc
    echo "main ; corpus file: ${corpus} ; n: ${n_val} ; prefix: $prefix" >> ${outd}/lm_desc
}

########
if [ $# -lt 1 ]; then
    print_desc
    exit 1
fi

# Read parameters
pr_given=0
pr_val=1
c_given=0
o_given=0
n_given=0
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
        "-c") shift
            if [ $# -ne 0 ]; then
                corpus=$1
                c_given=1
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
                n_val=$1
                n_given=1
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
if [ ${c_given} -eq 0 ]; then
    echo "Error! -c parameter not given" >&2
    exit 1
else
    if [ ! -f ${corpus} ]; then
        echo "Error! file ${corpus} does not exist" >&2
        exit 1            
    else
        # Obtain absolute path
        corpus=`get_absolute_path $corpus`
    fi
fi

if [ ${o_given} -eq 0 ]; then
    echo "Error! -o parameter not given!" >&2
    exit 1
else
    if [ -d ${outd}/main ]; then
        echo "Warning! output directory does exist" >&2 
#        echo "Error! output directory should not exist" >&2 
#        exit 1
    else
        mkdir -p ${outd}/main || { echo "Error! cannot create output directory" >&2; exit 1; }
    fi
    # Obtain absolute path
    outd=`get_absolute_path $outd`
fi

if [ ${n_given} -eq 0 ]; then
    echo "Error! -n parameter not given!" >&2
    exit 1
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
prefix=$outd/main/trg.lm
${bindir}/thot_pbs_get_ngram_counts -pr ${pr_val} \
    -c $corpus -o $prefix -n ${n_val} ${unk_opt} \
    ${qs_opt} "${qs_par}" -tdir $tdir -sdir $sdir ${debug_opt} || exit 1

# Generate weights file
n_buckets=3
bsize=10
${bindir}/thot_gen_init_file_with_jmlm_weights ${n_val} ${n_buckets} ${bsize} > $prefix.weights

# Generate wp file
nlines_wp_file=100000
${bindir}/thot_shuffle 31415 $corpus | $HEAD -${nlines_wp_file} > $prefix.wp

# Create descriptor file
create_desc_file $outd
