# Author: Daniel Ortiz Mart\'inez
# *- bash -*

########
if [ $# -lt 1 ]; then
    echo "thot_recase -f <string> -r <string> [-tdir <string>]"
    echo ""
    echo "-f <string>     File with text to be recased (can be read from standard"
    echo "                input)"
    echo "-r <string>     File with raw text in the language of interest"
    echo "-tdir <string>  Directory for temporary files (/tmp by default)"
else
    
    # Read parameters
    f_given=0
    r_given=0
    tdir=/tmp
    while [ $# -ne 0 ]; do
        case $1 in
        "-f") shift
            if [ $# -ne 0 ]; then
                ffile=$1
                f_given=1
            fi
            ;;
        "-r") shift
            if [ $# -ne 0 ]; then
                rfile=$1
                r_given=1
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
    if [ ${f_given} -eq 0 ]; then
        echo "Error! -f parameter not given" >&2
        exit 1
    fi

    if [ ! -f ${ffile} ]; then
        echo "Error! ${ffile} file does not exist" >&2
        exit 1
    fi

    if [ ${r_given} -eq 0 ]; then
        echo "Error! -r parameter not given" >&2
        exit 1
    fi

    if [ ! -f ${rfile} ]; then
        echo "Error! ${rfile} file does not exist" >&2
        exit 1
    fi

    ## Process parameters
    
    # Create directory for temporary files
    TMPDIR=`mktemp -d $tdir/thot_detokenize_tdir_XXXXX`
    trap "rm -rf $TMPDIR 2>/dev/null" EXIT

    ## Train models
    
    # Train translation model
    $bindir/thot_train_rec_tm -r ${rfile} -o $TMPDIR/models

    # Train language model
    $bindir/thot_train_rec_lm_mr -r ${rfile} -n 3 -o $TMPDIR/models -tdir $tdir

    # Tune weights
    # TBD

    # Recase sentences
    $bindir/thot_rec_translator -f ${ffile} -m $TMPDIR/models -w "0 0 0 1" 

fi
