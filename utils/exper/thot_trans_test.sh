# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Automatically obtains statistical machine translation results.

########
print_desc()
{
    echo "thot_trans_test written by Daniel Ortiz"
    echo "thot_trans_test translates a test corpus using the output of thot_auto_smt"
    echo "type \"thot_trans_test --help\" to get usage information"
}

########
version()
{
    echo "thot_trans_test is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_trans_test         [-pr <int>]"
    echo "                        -d <string> -t <string> [--tok] [--lower]"
    echo "                        [-qs <string>] [-tdir <string>]"
    echo "                        [-sdir <string>] [-debug] [--help] [--version]"
    echo ""
    echo "-pr <int>               Number of processors (1 by default)"
    echo "-d <string>             Path to output directory generated with thot_auto_smt"
    echo "-t <string>             File with test corpus to be translated"
    echo "--tok                   Execute tokenization stage"
    echo "--lower                 Execute lowercasing stage"
    echo "--no-trans              Do not generate translations"
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
tok_corpus()
{
    # Tokenize corpus
    echo "**** Tokenizing corpus" >&2
    suff="tok"
    ${bindir}/thot_tokenize -f ${test_corpus} \
        > ${outd}/${preproc_dir}/src_${suff}.test 2>>${outd}/${preproc_dir}/thot_tokenize.log || exit 1
    echo "" >&2

    # Redefine corpus variables
    test_corpus=${outd}/${preproc_dir}/src_${suff}.test
}

########
lowercase_corpus()
{
    # Tokenize corpus
    echo "**** Lowercasing corpus" >&2
    if [ ${tok_given} -eq 0 ]; then
        suff="lc"
    else
        suff="tok_lc"
    fi
    ${bindir}/thot_lowercase -f ${test_corpus} \
        > ${outd}/${preproc_dir}/src_${suff}.test 2>>${outd}/${preproc_dir}/thot_lowercase.log || exit 1
    echo "" >&2

    # Redefine corpus variables
    test_corpus=${outd}/${preproc_dir}/src_${suff}.test
}


########
recase_output()
{
    echo "**** Recasing output" >&2

    # Determine basic raw files
    if [ ${tok_given} = 0 ]; then
        raw_src_pref=${scorpus_pref}
        raw_trg_pref=${tcorpus_pref}
    else
        raw_src_pref=${thot_auto_smt_dir}/preproc_data/src_trg/trg_tok
        raw_trg_pref=${thot_auto_smt_dir}/preproc_data/src_trg/src_tok
    fi

    # Generate raw text file for recasing
    ${bindir}/thot_gen_rtfile -s ${raw_src_pref} \
        -t ${raw_trg_pref} -tdir $tdir > $tdir/rfile_rec
      
    # Recase output
    ${bindir}/thot_recase -f ${output_file} -r $tdir/rfile_rec -w \
        -tdir $tdir > ${output_file}_rec 2> ${thot_auto_smt_dir}/output/${transoutd}/thot_recase.log
    echo "" >&2

    # Remove temporary files
    rm $tdir/rfile_rec

    # Redefine output_file variable
    output_file=${output_file}_rec
}

########
detok_output()
{
    echo "**** Detokenizing output" >&2

    # Generate raw text file for recasing
    ${bindir}/thot_gen_rtfile -s ${scorpus_pref} \
        -t ${tcorpus_pref} -tdir $tdir > $tdir/rfile_detok

    # Detokenize output
    ${bindir}/thot_detokenize -f ${output_file} -r $tdir/rfile_detok \
        -tdir $tdir > ${output_file}_detok 2> ${thot_auto_smt_dir}/output/${transoutd}/thot_detokenize.log
    echo "" >&2

    # Remove temporary files
    rm $tdir/rfile_detok

    # Redefine output_file variable
    output_file=${output_file}_detok
}

########
if [ $# -lt 1 ]; then
    print_desc
    exit 1
fi

# Read parameters
pr_given=0
pr_val=1
d_given=0
t_given=0
tok_given=0
lower_given=0
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
        "-pr") shift
            if [ $# -ne 0 ]; then
                pr_val=$1
                pr_given=1
            fi
            ;;
        "-d") shift
            if [ $# -ne 0 ]; then
                thot_auto_smt_dir=$1
                d_given=1
            fi
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                test_corpus=$1
                t_given=1
            fi
            ;;
        "--tok") tok_given=1
            ;;
        "--lower") lower_given=1
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
if [ ${d_given} -eq 0 ]; then
    echo "Error! -d parameter not given" >&2
    exit 1
else
    if [ ! -f ${thot_auto_smt_dir}/input_pars.txt ]; then
        echo "Error! thot_auto_smt output directory appears to be incorrect"
        exit 1
    fi
fi

# Obtain absolute path of initial source files
scorpus_pref=`cat ${thot_auto_smt_dir}/input_pars.txt | $GREP "\-s is" | $AWK '{printf"%s",$3}'`
scorpus_pref=`get_absolute_path $scorpus_pref`

# Complete prefix of source files
scorpus_train=${scorpus_pref}.train
scorpus_dev=${scorpus_pref}.dev
scorpus_test=${scorpus_pref}.test

# Check existence of files
#for file in ${scorpus_train} ${scorpus_dev} ${scorpus_test}; do
for file in ${scorpus_train}; do
    if [ ! -f ${file} ]; then
        echo "Error! file ${file} does not exist" >&2
        exit 1
    fi
done

# Obtain absolute path of initial target files
tcorpus_pref=`cat ${thot_auto_smt_dir}/input_pars.txt | $GREP "\-t is" | $AWK '{printf"%s",$3}'`
tcorpus_pref=`get_absolute_path $tcorpus_pref`

# Complete prefix of target files
tcorpus_train=${tcorpus_pref}.train
tcorpus_dev=${tcorpus_pref}.dev
tcorpus_test=${tcorpus_pref}.test

# Check existence of files
#for file in ${tcorpus_train} ${tcorpus_dev} ${tcorpus_test}; do
for file in ${tcorpus_train}; do
    if [ ! -f ${file} ]; then
        echo "Error! file ${file} does not exist" >&2
        exit 1
    fi
done

# Check parameters
if [ ${t_given} -eq 0 ]; then
    echo "Error! -t parameter not given" >&2
    exit 1
else
    if [ ! -f ${test_corpus} ]; then
        echo "Error! test file with sentences to be translated does not exist"
        exit 1
    fi
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

# Obtain absolute path of ${test_corpus}
test_corpus=`get_absolute_path ${test_corpus}`

# Obtain basename of ${test_corpus}
base_tc=`$BASENAME ${test_corpus}`

# Create preproc dir if necessary
if [ ${tok_given} -eq 1 -o ${lower_given} -eq 1 -o ${skip_clean_given} -eq 0 ]; then
    # Store preproc dir name in a variable
    preproc_dir=preproc_data/${base_tc}
    # Check if the directory exists
    if [ ! -d ${outd}/${preproc_dir} ]; then
        mkdir -p ${outd}/${preproc_dir} || exit 1
    fi
fi

# Tokenize corpus if requested
if [ ${tok_given} -eq 1 ]; then
    tok_corpus
fi

# Lowercase corpus if requested
if [ ${lower_given} -eq 1 ]; then
    lowercase_corpus
fi

# Translate test corpus if requested

# Create dir for model filtering
if [ ! -d ${outd}/output/$curr_date ]; then
    mkdir -p ${outd}/filtered_models || exit 1
fi

# Prepare system to translate test corpus
if [ -f ${test_corpus} -a -f ${thot_auto_smt_dir}/smt_tune/tuned_for_dev.cfg ]; then
    echo "**** Preparing system to translate test corpus" >&2
    ${bindir}/thot_prepare_sys_for_test -c ${thot_auto_smt_dir}/smt_tune/tuned_for_dev.cfg -t ${test_corpus}  \
        -o ${thot_auto_smt_dir}/filtered_models/${base_tc} ${qs_opt} "${qs_par}" -tdir $tdir -sdir $sdir || exit 1
    echo "" >&2
fi

# Obtain current date
curr_date=`date '+%Y_%m_%d'`

# Create variable containing traslator output dir name
transoutd=${base_tc}.${curr_date}

# Create translator output dir
if [ ! -d ${outd}/output/${transoutd} ]; then
    mkdir -p ${outd}/output/${transoutd} || exit 1
fi

# Generate translations
if [ -f ${test_corpus} -a -f ${thot_auto_smt_dir}/smt_tune/tuned_for_dev.cfg ]; then
    echo "**** Translating test corpus" >&2
    ${bindir}/thot_decoder -pr ${pr_val} -c ${thot_auto_smt_dir}/filtered_models/${base_tc}/test_specific.cfg \
        -t ${test_corpus} -o ${thot_auto_smt_dir}/output/${transoutd}/thot_decoder_out ${debug_opt} -v || exit 1
    echo "" >&2
fi

### Execute post-processing steps if required

# Define output_file variable
output_file=${thot_auto_smt_dir}/output/${transoutd}/thot_decoder_out

# Recasing stage
if [ ${lower_given} = 1 ]; then
    # Recase
    recase_output
fi

# Detokenization stage
if [ ${tok_given} = 1 ]; then
    # Detokenize
    detok_output
fi
