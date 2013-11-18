# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Given a translation table composed of entries in the following format:
# s_phrase ||| t_phrase ||| c_s c_t c_srctrg
# like those generated with sum_st_counts,
# obtains a weighted table which is based on weights for the direct and
# inverse probabilities given as parameters.


if [ $# -ne 2 -a $# -ne 3 ]; then
echo "Usage: thot_weigh_ttable <pt_s_weight> <ps_t_weight> <ttable>"
else

if [ $# -eq 2 ];then

    pstw=$1
    ptsw=$2
    ttable=""
    
else

    pstw=$1
    ptsw=$2
    ttable=$3

fi

LC_NUMERIC="C" ${AWK} -v pstw=$pstw -v ptsw=$ptsw 'BEGIN{
           count_s=0
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
           c_s=$(NF-2)
           c_t=$(NF-1)
           c_st=$(NF)
           if(ptsw==0)
           {
             # print pst
             probst=(c_st/c_t)*pstw
             if(probst<0.00000001) probst=0.00000001
             printf"%s ||| %s ||| %.8f\n",src,trg,probst
           }
           else
           {
             if(pstw==0)
             {
               # print pts
               probts=(c_st/c_s)*ptsw
               if(probts<0.00000001) probts=0.00000001
               printf"%s ||| %s ||| %.8f\n",src,trg,probts
             }
             else
             {
               # print pts and pst
               probts=(c_st/c_s)*ptsw
               if(probts<0.00000001) probts=0.00000001
               probst=(c_st/c_t)*pstw
               if(probst<0.00000001) probst=0.00000001
               printf"%s ||| %s ||| %.8f %.8f\n",src,trg,probts,probst
             }
           }
          }' $ttable

fi
