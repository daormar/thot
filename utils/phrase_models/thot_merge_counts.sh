# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# merge the counts of a ttable generated by thot

if [ $# -ne 0 -a $# -ne 1 ]; then
echo "Usage: thot_merge_counts"
else

"${AWK}" '\
     function delete_array(array)
     {
      delete array
     }
     function print_counts()
     {
       first_trg=trgphrase[1] ""
       gc_srctrg=c_srctrg[1]
       for(n=2;n<=num_trg_phrases;++n)
       {
         if(first_trg==trgphrase[n])
         {
           # Accumulate count of target phrase for additional chunk
           gc_srctrg+=c_srctrg[n]
         }
         else
         {
           # Print count for current target phrase
           printf"%s ||| %s ||| %.8f %.8f\n",first_src,first_trg,count_s,gc_srctrg

           # Initialize variables for next target phrase
           first_trg=trgphrase[n] ""
           gc_srctrg=c_srctrg[n]
          }
       }
       # Print last target phrase
       printf"%s ||| %s ||| %.8f %.8f\n",first_src,first_trg,count_s,gc_srctrg
     }
     BEGIN{
           count_s=0
           first_entry=1
          }
          {
           # extract source phrase
           src=$1 ""
           for(i=2;i<=NF;++i)
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
           for(;i<=NF-4;++i)
           {
             ++num_trg_words
             trg=trg" "$i
           }
           c_st=$(NF-1)

           # verify if it is the first entry of the table
           if(first_entry==1)
           {
             first_src=src
             first_entry=0
             num_trg_phrases=1
             trgphrase[num_trg_phrases]=trg
             c_srctrg[num_trg_phrases]=c_st
           }
           else
           {
             # a new source phrase has appeared?
             if(first_src!=src)
             {
               # print counts
               print_counts()

               # reset variables
               first_src=src
               delete_array(chunks)
               delete_array(trgphrase) 
               delete_array(c_srctrg) 
               num_trg_phrases=1
               trgphrase[num_trg_phrases]=trg
               c_srctrg[num_trg_phrases]=c_st
               count_s=0
             }
             else
             {
               ++num_trg_phrases
               trgphrase[num_trg_phrases]=trg
               c_srctrg[num_trg_phrases]=c_st
             }
           }
           if(!($NF in chunks)) 
           {
             chunks[$NF]=$(NF-2)
             count_s+=$(NF-2)
           }
          }
       END{
             # print last group of counts
             print_counts()
          }' "$1"
fi
