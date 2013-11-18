# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Reads a thot ttable from the standard input 
# or from a given file, and swaps s and t
# NOTE: here, source phrases are those that appears in first place in
# the phrase table

if [ $# -ne 0 -a $# -ne 1 ]; then
    echo "Usage: thot_flip_phr [<thot_ttable(can be read from stdin)>]"
else
 
 file=$1
 ${AWK}              'BEGIN{ 
                            num_lines=0
                           }
                           {
                            ++num_lines
                            num_src_words=0
                            num_trg_words=0
                            src=""
                            trg=""
                            countTrg=1
                            for(i=1;i<NF;++i)
                            {
                              if($i != "|||")
                              {
                               if(countTrg==1)
                               {
                                ++num_trg_words
                                trg=trg" "$i
                               }
                               else
                               {
                                ++num_src_words
                                if(src=="") src=$i
                                else src=src" "$i
                               }
                              }
                              else
                              {
                               if(countTrg==0) 
                               {
                                last_i=i+1
                                break
                               }
                               if(countTrg==1) countTrg=0
                              }
                            }
                            printf"%s |||%s |||",src,trg
                            for(i=last_i;i<=NF;++i)
                                printf" %s",$i
                            printf("\n")
		           }' ${file}
		  
fi
