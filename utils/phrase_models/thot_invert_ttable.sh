# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Inverts a ttable obtained with thot using -pc option

if [ $# -ne 0 -a $# -ne 2 -a $# -ne 4 ]; then
    echo "Usage: thot_invert_ttable [-t <thot_pc_ttable(can be read from stdin)>]"
    echo "                          [-T <tmpdir>]"
else

    t_given=0
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

    if test ${sortT} = "yes"; then
        SORT_TMP="-T $tmpdir"
    else
        SORT_TMP=""
    fi

    export LC_ALL=""
    export LC_COLLATE=C

    if [ ${t_given} -eq 1 ]; then
        ${bindir}/thot_flip_phr ${ttable_file} | ${SORT} ${SORT_TMP} -t " " ${sortpars} \
            | ${bindir}/thot_sum_st_counts | ${bindir}/thot_weigh_ttable 0 1
    else
        ${bindir}/thot_flip_phr | ${SORT} ${SORT_TMP} -t " " ${sortpars} \
            | ${bindir}/thot_sum_st_counts | ${bindir}/thot_weigh_ttable 0 1
    fi
fi
