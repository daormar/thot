# Author: Daniel Ortiz Mart\'inez
# *- bash -*

##################
replace_oov_with_unk()
{
    local_vocfile=$1
    "$AWK" -v vocfile="${local_vocfile}" 'BEGIN{
           while( (getline <vocfile) > 0)
           {
            voc[$1]=1
           }
          }
          {
            for(i=1;i<=NF;++i)
            {
             if(!($i in voc))
               $i="<unk>"
            }
            print $0
          }'
}

##################
if [ $# -lt 1 ]; then
    echo "Use: thot_replace_oov_with_unk -v <string>"
    echo ""
    echo "-v <string>    : Vocabulary file."
    echo ""
else    
    # Read parameters
    v_given=0
    while [ $# -ne 0 ]; do
        case $1 in
            "-v") shift
                v_given=1
                vocfile=$1
                ;;
        esac
        shift
    done

    # Check parameters
    if [ ${v_given} -eq 0 ]; then
        echo "Error: -v option not given" >&2
    fi

    if [ ! -f $vocfile ]; then
        echo "Error: file $vocfile does not exist" >&2
        exit 1
    fi

    # Process parameters
    replace_oov_with_unk "$vocfile"

fi
