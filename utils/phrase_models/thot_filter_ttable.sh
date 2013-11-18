# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Filters a translation table given a test corpus and an integer value
# that sets the maximum number of translation options for each target
# phrase that are considered during a translation process.

N_DEFAULT=20
if [ $# -ne 2 -a $# -ne 4 -a $# -ne 6 -a $# -ne 8 ]; then
    echo "Usage: thot_filter_ttable [-t <string>]"
    echo "                          -c <string> [-n <int>] [-T <string>]"
    echo ""
    echo "-t <thot_pc_ttable> : Thot translation table obtained with the -pc option"
    echo "                      (can be read from stdin)."
    echo "-c <test-corpus>    : Test corpus to be translated."
    echo "-n <int>            : Maximum number of translation options for each target"
    echo "                      phrase that are considered during a translation process"
    echo "                      ("${N_DEFAULT}" by default)."
    echo "-T <tmpdir>         : Use <tmpdir> for temporaries instead of /tmp"
    echo "                      during the generation of the phrase model"
else

    t_given=0
    ttable_file=""
    c_given=0
    n_val=${N_DEFAULT}
    tmpdir="/tmp"
    while [ $# -ne 0 ]; do
        case $1 in
            "-t") shift
                if [ $# -ne 0 ]; then
                    ttable_file=$1
                    t_given=1
                fi
                ;;
            "-c") shift
                if [ $# -ne 0 ]; then
                    test_corpus_file=$1
                    c_given=1
                fi
                ;;
            "-n") shift
                if [ $# -ne 0 ]; then
                    n_val=$1
                fi
                ;;
            "-T") shift
                if [ $# -ne 0 ]; then
                    tmpdir=$1
                else
                    tmpdir="/tmp"
                fi
                ;;
        esac
        shift
    done

    if [ ${t_given} -eq 1 ]; then
        if [ ! -f ${ttable_file} ]; then
            echo "Error: file ${ttable_file} does not exist!"
            exit 1
        fi        
    fi

    if [ ${c_given} -eq 0 ]; then
        echo "Error: test corpus not given!"
        exit 1
    else
        if [ ! -f ${test_corpus_file} ]; then
            echo "Error: file ${test_corpus_file} does not exist!"
            exit 1
        fi
    fi

    $bindir/thot_filter_ttable_given_corpus ${ttable_file} ${test_corpus_file} |\
        $bindir/thot_get_nbest_for_trg -n ${n_val} -p -T ${tmpdir}
fi
