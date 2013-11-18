# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Like put_st_lengths, but it put the counts between the phrase pairs.
# This script is used to prune or sort the ttable.

if [ $# -ne 0 -a $# -ne 1 ]; then
    echo "Usage: thot_put_st_lengths2"
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
                            countSrc=1
                            for(i=1;i<NF;++i)
                            {
                              if($i != "|||")
                              {
                               if(countSrc==1)
                               {
                                ++num_src_words
                                if(src=="") src=$i
                                else src=src" "$i
                               }
                               else
                               {
                                ++num_trg_words
                                if(trg=="") trg=$i
                                else trg=trg" "$i
                               }
                              }
                              else
                              {
                               if(countSrc==0) 
                               {
                                last_i=i+1
                                break
                               }
                               if(countSrc==1) countSrc=0
                              }
                            }
                            printf"%03d %s |||",num_src_words,src
                            printf" %.20f %03d %s",(1/$NF),num_trg_words,trg
                            printf" ||| %s %s",$(NF-1),$(NF)
                            printf("\n")
		           }' ${file}
		  
fi
