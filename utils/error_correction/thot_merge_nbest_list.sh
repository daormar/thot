# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Merges two n-best list files.

##########
obtain_nbl_weight_names()
{
    local_file=$1
    $AWK -F " [|][|][|] " '
        {
         if(NR==1)
         {
          # Obtain weight names
          numw=split($1,line_arr," , ")
          for(i=1;i<=numw;++i)
          {
           winfolen=split(line_arr[i],weight_info," ")
           weight_name[i]=weight_info[winfolen-1]
          }
          # Print weight names
          for(i=1;i<=numw;++i)
          {
            printf "%s",weight_name[i]
            if(i!=numw)
             printf" "
          }
         }
        }' ${local_file}
}

##########
nbls_have_same_weights()
{
    if [ "$nbl1_weight_names" = "$nbl2_weight_names" ]; then
        echo 1
    else
        echo 0
    fi
}

##########
print_nblist_header()
{
    echo ${nbl1_weight_names} | \
        $AWK '{
               printf "# "
               for(i=1;i<=NF;++i)
               {
                printf"%s -",$i
                if(i!=NF)
                 printf" , "
               }
               printf"\n"
              }'
}

##########
print_nblist_entries()
{
    cat $file1 $file2 | ${AWK} '{if($1!="#") {$1="-"; printf"%s\n",$0}}' | \
        LC_ALL=C ${SORT} | $UNIQ | $AWK '{printf"%s\n",$0}'
}

##########
if [ $# -lt 2 ]; then
    echo "Usage: thot_merge_nbest_list <nblfile1> <nblfile2>"
else
    # Take parameters
    file1=$1
    file2=$2

    # Verify parameters
    if [ ! -f ${file1} ]; then
        echo "Error: file ${file1} does not exist" >&2
    fi

    if [ ! -f ${file2} ]; then
        echo "Error: file ${file2} does not exist" >&2
    fi

    # Obtain weight names
    nbl1_weight_names="`obtain_nbl_weight_names $file1`"
    nbl2_weight_names="`obtain_nbl_weight_names $file2`"

    # Verify that the two n-best lists have the same weights
    ret=`nbls_have_same_weights`
    if [ $ret -eq 0 ]; then
        echo "Error: n-best lists have different sets of weights" >&2
    fi
    
    # Print header
    print_nblist_header
    
    # Print merged n-best list entries
    print_nblist_entries
fi
