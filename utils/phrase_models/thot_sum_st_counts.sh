# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# sum the counts for the (source,target) counts of a model trained by
# thot, for all contiguous source phrases. This script is internally
# used by the invert_ttable.sh script.
# The output has the following format:
# s_phrase ||| t_phrase ||| c_s c_t c_srctrg
# NOTE: here, the source phrase is the one that appears in first place

if [ $# -ne 0 -a $# -ne 1 ]; then
    echo "Usage: thot_sum_st_counts <flipped_sorted_thot_pc_ttable>"
else

    LC_NUMERIC="C" ${AWK} 'BEGIN{
           sum_st_counts=0
          }
          {
           # extract source phrase
           src=$1 ""
           for(i=2;i<NF;++i)
           {
             if($i != "|||")
             {
               ++num_src_words
               src=src" "$i
             }
             else
             {
               break
             }
           }
           # extract target phrase
           ++i; trg=$i; ++i;
           for(;i<NF;++i)
           {
             if($i != "|||")
             {
               ++num_trg_words
               trg=trg" "$i
             }
             else
             {
               break
             }
           }
           c_s=$(NF-1)
           c_st=$(NF)

           # verify if it is the first time that "src" appears
           if(sum_st_counts==0)
           {
             first_src=src
             num_trg_phrases=1
             trgphrase[num_trg_phrases]=trg
             c_srctrg[num_trg_phrases]=c_st
             c_src[num_trg_phrases]=c_s
           }
           else
           {
             # a new source phrase has appeared?
             if(first_src!=src)
             {
               # print counts
               for(n=1;n<=num_trg_phrases;++n)
               {
                 printf"%s ||| %s ||| %.8f %.8f %.8f\n",first_src,trgphrase[n],c_src[n],sum_st_counts,c_srctrg[n]
               }
               # reset variables
               first_src=src
               for(i in trgphrase) delete trgphrase[i] 
               for(i in c_srctrg) delete c_srctrg[i] 
               num_trg_phrases=1
               trgphrase[num_trg_phrases]=trg
               c_srctrg[num_trg_phrases]=c_st
               c_src[num_trg_phrases]=c_s
               sum_st_counts=0
             }
             else
             {
               ++num_trg_phrases
               trgphrase[num_trg_phrases]=trg
               c_srctrg[num_trg_phrases]=c_st
               c_src[num_trg_phrases]=c_s
             }
           }
           sum_st_counts+=$(NF)
          }
       END{
             # print last group of counts
             for(n=1;n<=num_trg_phrases;++n)
             {
               printf"%s ||| %s ||| %.8f %.8f %.8f\n",first_src,trgphrase[n],c_src[n],sum_st_counts,c_srctrg[n]
             }
          }' $1

fi
