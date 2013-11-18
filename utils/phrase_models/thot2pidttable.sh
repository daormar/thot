# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Obtains a inverse-direct translation table from a ttable obtained with thot using -pc option

if [ $# -ne 0 -a $# -ne 2 -a $# -ne 4 ]; then
    echo "Usage: thot2pidttable [-t <thot_pc_ttable(can be read from stdin)>]" 
    echo "                      [-T <tmpdir>]"
else

    t_given=0
    tmpdir="/tmp"
    while [ $# -ne 0 ]; do
        case $1 in
            "-t") shift
                if [ $# -ne 0 ]; then
                    ttable_file=$1
                    t_given=1
                else
                    t_given=0
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

    if test ${sortT} = "yes"; then
        SORT_TMP="-T $tmpdir"
    else
        SORT_TMP=""
    fi

    export LC_ALL=""
    export LC_COLLATE=C

    if [ ${t_given} -eq 1 ]; then
        ${bindir}/thot_flip_phr ${ttable_file} | sort ${SORT_TMP} -t " " ${sortpars} \
            | ${bindir}/thot_sum_st_counts | grep -v "<UNUSED_WORD>" | ${bindir}/thot_weigh_ttable 1 1
    else
        ${bindir}/thot_flip_phr | sort ${SORT_TMP} -t " " ${sortpars} \
            | ${bindir}/thot_sum_st_counts | grep -v "<UNUSED_WORD>" | ${bindir}/thot_weigh_ttable 1 1
    fi
fi
