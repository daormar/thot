# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Automatically obtains statistical machine translation results.

########
print_desc()
{
    echo "thot_auto_smt written by Daniel Ortiz"
    echo "thot_auto_smt automatically obtains statistical machine translation results"
    echo "type \"thot_auto_smt --help\" to get usage information"
}

########
version()
{
    echo "thot_auto_smt is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_auto_smt           [-pr <int>]"
    echo "                        -s <string> -t <string> -o <string>"
    echo "                        [-qs <string>] [-tdir <string>]"
    echo "                        [-sdir <string>] [-debug] [--help] [--version]"
    echo ""
    echo "-pr <int>               Number of processors (1 by default)"
    echo "-s <string>             Prefix of files with source sentences (the"
    echo "                        following suffixes are assumed: .train, .dev and .test)"
    echo "-t <string>             Prefix of files with target sentences (the"
    echo "                        following suffixes are assumed: .train, .dev and .test)"
    echo "-o <string>             Output directory common to all processors."
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
pr_given=0
pr_val=1
s_given=0
t_given=0
o_given=0
n_given=0
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
        "-s") shift
            if [ $# -ne 0 ]; then
                scorpus_pref=$1
                s_given=1
            fi
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                tcorpus_pref=$1
                t_given=1
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                outd=$1
                o_given=1
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
if [ ${s_given} -eq 0 ]; then
    echo "Error! -s parameter not given" >&2
    exit 1
else
    # Obtain absolute path
    scorpus_pref=`get_absolute_path $scorpus_pref`

    # Complete prefix of source files
    scorpus_train=${scorpus_pref}.train
    scorpus_dev=${scorpus_pref}.dev
    scorpus_test=${scorpus_pref}.test

    # Check existence of files
    for file in ${scorpus_train} ${scorpus_dev} ${scorpus_test}; do
        if [ ! -f ${file} ]; then
            echo "Error! file ${file} does not exist" >&2
            exit 1
        fi
    done
fi

if [ ${t_given} -eq 0 ]; then        
    echo "Error! -t parameter not given" >&2
    exit 1
else
    # Obtain absolute path
    tcorpus_pref=`get_absolute_path $tcorpus_pref`

    # Complete prefix of target files
    tcorpus_train=${tcorpus_pref}.train
    tcorpus_dev=${tcorpus_pref}.dev
    tcorpus_test=${tcorpus_pref}.test

    # Check existence of files
    for file in ${tcorpus_train} ${tcorpus_dev} ${tcorpus_test}; do
        if [ ! -f ${file} ]; then
            echo "Error! file ${file} does not exist" >&2
            exit 1
        fi
    done

fi

# Check that source and target files are parallel
nl_source=`wc -l $scorpus_train | $AWK '{printf"%d",$1}'`
nl_target=`wc -l $tcorpus_train | $AWK '{printf"%d",$1}'`
if [ ${nl_source} -ne ${nl_target} ]; then
    echo "Error! source and target training files have not the same number of lines" >&2 
    exit 1
fi

nl_source=`wc -l $scorpus_dev | $AWK '{printf"%d",$1}'`
nl_target=`wc -l $tcorpus_dev | $AWK '{printf"%d",$1}'`
if [ ${nl_source} -ne ${nl_target} ]; then
    echo "Error! source and target development files have not the same number of lines" >&2 
    exit 1
fi

nl_source=`wc -l $scorpus_test | $AWK '{printf"%d",$1}'`
nl_target=`wc -l $tcorpus_test | $AWK '{printf"%d",$1}'`
if [ ${nl_source} -ne ${nl_target} ]; then
    echo "Error! source and target test files have not the same number of lines" >&2 
    exit 1
fi

if [ ${o_given} -eq 0 ]; then
    echo "Error! -o parameter not given!" >&2
    exit 1
else
    if [ -d ${outd} ]; then
        echo "Warning! output directory does exist" >&2 
        # echo "Error! output directory should not exist" >&2 
        # exit 1
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

# Train models

if [ -f ${scorpus_train} -a -f ${tcorpus_train} ]; then

    # Train language model
    echo "**** Training language model" >&2
    ${bindir}/thot_lm_train -pr ${pr_val} -c ${tcorpus_train} -o ${outd}/lm -n 4 -unk \
        ${qs_opt} "${qs_par}" -tdir $tdir -sdir $sdir ${debug_opt} || exit 1

    # Train translation model
    echo "**** Training translation model" >&2
    ${bindir}/thot_tm_train -pr ${pr_val} -s ${scorpus_train} -t ${tcorpus_train} -o ${outd}/tm -n 5 \
        ${qs_opt} "${qs_par}" -tdir $tdir -sdir $sdir ${debug_opt} || exit 1

else
    echo "Error! training files do not exist" >&2
    exit 1                
fi

# Generate cfg file

echo "**** Generating configuration file" >&2
${bindir}/thot_gen_cfg_file $outd/lm/lm_desc $outd/tm/tm_desc > $outd/server.cfg || exit 1
echo "" >&2

# Tune parameters

if [ -f ${scorpus_dev} -a -f ${tcorpus_dev} ]; then
    echo "**** Tuning model parameters" >&2
    ${bindir}/thot_smt_tune -pr ${pr_val} -c $outd/server.cfg -s ${scorpus_dev} -t ${tcorpus_dev} -o $outd/tune ${qs_opt} \
        ${qs_opt} "${qs_par}" -tdir $tdir -sdir $sdir ${debug_opt} || exit 1
    tuning_executed="yes"
fi

# Prepare system to translate test corpus

if [ -f ${scorpus_test} -a -f ${tcorpus_test} -a ${tuning_executed} = "yes" ]; then
    echo "**** Preparing system to translate test corpus" >&2
    ${bindir}/thot_prepare_sys_for_test -c $outd/tune/tuned_for_dev.cfg -t ${scorpus_test} \
        -o $outd/systest ${qs_opt} "${qs_par}" -tdir $tdir -sdir $sdir || exit 1
    echo "" >&2
fi

# Translate test corpus

if [ -f ${scorpus_test} -a -f ${tcorpus_test} -a ${tuning_executed} = "yes" ]; then
    echo "**** Translating test corpus" >&2
${bindir}/thot_decoder -pr ${pr_val} -c $outd/systest/test_specific.cfg \
        -t ${scorpus_test} -o $outd/thot_decoder_out ${debug_opt} || exit 1
    test_trans_executed="yes"
    echo "" >&2
fi

# Obtain BLEU score

if [ ${test_trans_executed} = "yes" ]; then
    echo "**** Obtaining BLEU score" >&2
${bindir}/thot_calc_bleu -r ${tcorpus_test} -t $outd/thot_decoder_out > $outd/test.out.bleu || exit 1
    echo "" >&2
fi
