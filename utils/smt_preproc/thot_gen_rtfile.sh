# Author: Daniel Ortiz Mart\'inez
# *- bash -*

########
if [ $# -lt 1 ]; then
    echo "thot_gen_rtfile [-s <string>] -t <string> [-tdir <string>]"
    echo ""
    echo "-s <string>     Prefix of files with source sentences (the following suffixes"
    echo "                are assumed: .test)"
    echo "-t <string>     Prefix of files with target sentences (the following suffixes"
    echo "                are assumed: .train and .dev)"
    echo "-tdir <string>  Directory for temporary files (/tmp by default)"
else
    
    # Read parameters
    s_given=0
    t_given=0
    tdir=/tmp
    while [ $# -ne 0 ]; do
        case $1 in
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
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1
            fi
            ;;
        esac
        shift
    done

    # Check parameters
    if [ ${s_given} -eq 1 ]; then
        # Complete prefix of source files
        scorpus_test=${scorpus_pref}.test

        # Check existence of files
        if [ ! -f ${scorpus_test} ]; then
            echo "Warning! file ${file} does not exist" >&2
        fi
    fi


    if [ ${t_given} -eq 0 ]; then
        echo "Error! -t parameter not given" >&2
        exit 1
    fi

    # Complete prefix of target files
    tcorpus_train=${tcorpus_pref}.train
    tcorpus_dev=${tcorpus_pref}.dev

    # Check existence of files
    if [ ! -f ${tcorpus_train} ]; then
        echo "Error! file ${file} does not exist" >&2
        exit 1
    fi

    if [ ! -f ${tcorpus_dev} ]; then
        echo "Warning! file ${file} does not exist" >&2
    fi

    ## Process parameters

    # Create directory for temporary files
    TMPDIR=`mktemp -d $tdir/thot_gen_rtfile_tdir_XXXXX`
    trap "rm -rf $TMPDIR 2>/dev/null" EXIT

    # Obtain raw text file
    
    # Obtain info from subset of target training corpus (this is done to
    # speed up computations)
    maxfsize=500000
    ${bindir}/thot_shuffle 31415 ${tcorpus_train} > $TMPDIR/tcorpus_train_shuff
    head -n ${maxfsize} $TMPDIR/tcorpus_train_shuff

    # Obtain info from subset of target dev corpus (with given subset
    # size, typically the whole corpus will be included)
    if [ -f ${tcorpus_dev} ]; then
        maxfsize=10000
        ${bindir}/thot_shuffle 31415 ${tcorpus_dev} > $TMPDIR/tcorpus_dev_shuff
        head -n ${maxfsize} $TMPDIR/tcorpus_dev_shuff
    fi

    # Obtain info from subset of source test corpus (with given subset
    # size, typically the whole corpus will be included)
    if [ ${s_given} -eq 1 ]; then
        if [ -f ${scorpus_test} ]; then
            maxfsize=10000
            ${bindir}/thot_shuffle 31415 ${scorpus_test} > $TMPDIR/scorpus_test_shuff
            head -n ${maxfsize} $TMPDIR/scorpus_test_shuff
        fi
    fi
fi
