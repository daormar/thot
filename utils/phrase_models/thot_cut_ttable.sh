# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# prune those model entries whose count for the source phrase is lower
# or equal to a given cutoff count

if [ $# -lt 2 ]; then
    echo "Usage: thot_cut_ttable [-t <string>] -c <int>"
    echo ""
    echo "-t <string>       table to be pruned (can be read from stdin)"
    echo ""
    echo "-c <int>          cutoff value for the count of source phrases" 
    echo ""
else
    
    t_given=0
    c_given=0
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
            "-c") shift
                if [ $# -ne 0 ]; then
                    c=$1
                    c_given=1
                else
                    echo "Error: a value for c must be given"
                    exit 1
                fi
                ;;
        esac
        shift
    done

    if [ ${c_given} -eq 0 ]; then
        echo "Error: c must be given"
        exit 1
    fi

    cat ${ttable_file} | ${AWK} -v c=$c '{if($(NF-1)>c) print $0}'
fi
