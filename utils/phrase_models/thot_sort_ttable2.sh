# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Sorts a given thot ttable in order to binarize it.

if [ $# -ne 0 -a $# -ne 2 -a $# -ne 4 ]; then
    echo "Usage: thot_sort_ttable2 [-t <thot_pc_ttable(can be read from stdin)>]"
    echo "                         [-T <tmpdir>]"
else

    t_given=0
    ttable_file=""
    tmpdir="/tmp"
    while [ $# -ne 0 ]; do
        case $1 in
            "-t") shift
                if [ $# -ne 0 ]; then
                    ttable_file=$1
                    t_given=1
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

    if test "${sortT}" = "yes"; then
        SORT_TMP="-T $tmpdir"
    else
        SORT_TMP=""
    fi

    cat ${ttable_file} | ${bindir}/thot_put_st_lengths2 | LC_ALL=C ${SORT} ${SORT_TMP} ${sortpars} | ${bindir}/thot_remove_st_lengths2
fi
