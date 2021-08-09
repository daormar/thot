# Author: Daniel Ortiz Mart\'inez
# *- bash -*

if [ $# -lt 1 ]; then
    echo "Use: thot_extract_sents_by_ln -n <string> -f <string> [-r]"
    echo ""
    echo "-n <string> : file with line numbers"
    echo "-f <string> : file with sentences to be extracted"
    echo "-r          : reverse mode, extract lines not present"
    echo ""
else
    
    # Read parameters
    n_given=0
    f_given=0
    r=0
    while [ $# -ne 0 ]; do
        case $1 in
        "-n") shift
            if [ $# -ne 0 ]; then
                fn=$1
                n_given=1
            fi
            ;;
        "-f") shift
            if [ $# -ne 0 ]; then
                file=$1
                f_given=1
            fi
            ;;
        "-r") r=1
            ;;
        esac
        shift
    done

    # Check parameters
    if [ ${n_given} -eq 0 ]; then
        echo "Error! -n parameter not given" >&2
        exit 1
    fi

    if [ ${f_given} -eq 0 ]; then
        echo "Error! -f parameter not given" >&2
        exit 1
    fi

    if [ ! -f "${fn}" ]; then
        echo "Error! file $fn does not exist" >&2
        exit 1
    fi

    # Process parameters
    "${AWK}" -v fn="$fn" -v r=$r \
        'BEGIN{
                while(getline < fn)
                {
                  sn[$1]=1
                }
                n=0
              }
              {
               ++n
               if(r==0)
               {
                if(n in sn)
                 printf"%s\n",$0
               }
               else
               {
                if(!(n in sn))
                 printf"%s\n",$0
               }
              }' "$file"
fi
