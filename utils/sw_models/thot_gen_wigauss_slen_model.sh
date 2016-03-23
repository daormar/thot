# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Generates an incremental gaussian sentence length model given a
# parallel corpus.

if [ $# -ne 2 ]; then
    echo "Usage: thot_gen_wigauss_slen_model <src_sent_file> <trg_sent_file>"
else
    src_file=$1
    trg_file=$2
    CUTOFF=5
    ${AWK} -v  src_file=${src_file} -v CUTOFF=${CUTOFF} ' BEGIN{
                   numSent=0
                   src_sen_len_sum=0
                   trg_sen_len_sum=0
                  }
                  {
                   ++numSent
                   trg_sen_len_sum+=NF
                   tlen=NF
                   getline <src_file
                   src_sen_len_sum+=NF
                   slen=NF;
                   weight=1
                   if(! (slen in k))
                   {
                     k[slen]=1
                     s[slen]=0
                     m[slen]=tlen
                     sw[slen]=weight
                   }
                   else
                   {
                     ++k[slen]
                     temp=weight+sw[slen]
                     s[slen]=s[slen]+(sw[slen]*weight*((tlen-m[slen])*(tlen-m[slen])))/temp
                     m[slen]=m[slen]+((tlen-m[slen])*weight)/temp
                     sw[slen]=temp
                   }
	          }
               END{
                   # print header
                   printf"Weighted incr. gaussian sentence length model...\n"
                   # print source and target average sentence info
                   printf"numsents: %d ; slensum: %d ; tlensum: %d\n",numSent,src_sen_len_sum,trg_sen_len_sum

                   # print results
                   for(i in m)
                   {
                     printf"%d %d %f %f %f\n",i,k[i],sw[i],m[i],s[i]
                   }
                  }' $trg_file
fi
