# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Remove the lengths of s and t that have been previously 
# put using "put_st_lengths2" script.
# The input is read from the standard input, or from a file.
# NOTE: here, the source phrase is the one that appears in first place
#       in the phrase table

if [ $# -ne 1 -a $# -ne 0 ]; then
echo "Usage: thot_remove_st_lengths <thot_ttable>"
else

 file=$1
 ${AWK} '{       
       count_src=0;
       for(i=2;i<=NF;++i)
       {
        if(count_src==0 && $i=="|||")
        {
         count_src=1; printf"||| "; 
         i+=3; 
         #i+=2;
        }
        if(i!=NF) 
        {
         printf "%s ",$i; 
        }
        else printf"%s\n",$i;
       }
      }' ${file}

fi
