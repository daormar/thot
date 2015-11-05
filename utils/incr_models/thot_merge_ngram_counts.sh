# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Given a file with counts that may be duplicated, prints a new file to
# the standard output where the duplicate counts are merged

if [ $# -ne 0 ]; then
echo "Usage: thot_merge_ngram_counts"
else

${AWK} '\
function delete_array(array)
{
  for (i in array) delete array[i]
}
     BEGIN{
           count_s=0
          }
          {
           # extract lm hist
           if(NF==4)
           {
            src="";
            i=1;
           }
           else
           {
            src=$1 ""
            for(i=2;i<=NF-4;++i)
            {
             ++num_src_words
             src=src" "$i
            }
           }
           # extract last word added
           trg=$i;
           ++i;
           num_trg_words=1
           c_st=$(NF-1)
           # verify if it is the first time that "src" appears
           if(count_s==0)
           {
             first_src=src
             num_trg_phrases=1
             trgphrase[num_trg_phrases]=trg
             c_srctrg[num_trg_phrases]=c_st
           }
           else
           {
             # a new lm hist has appeared?
             if(first_src!=src)
             {
               # print counts
               first_trg=trgphrase[1] ""
               gc_srctrg=c_srctrg[1]
               for(n=2;n<=num_trg_phrases;++n)
               {
                 if(first_trg==trgphrase[n])
                 {
                  gc_srctrg+=c_srctrg[n]
                 }
                 else
                 {
                  if(first_src=="") printf"%s %d %d\n",first_trg,count_s,gc_srctrg
                  else printf"%s %s %d %d\n",first_src,first_trg,count_s,gc_srctrg
                  first_trg=trgphrase[n] ""
                  gc_srctrg=c_srctrg[n]
                 }
               }
               if(first_src=="") printf"%s %d %d\n",first_trg,count_s,gc_srctrg
               else printf"%s %s %d %d\n",first_src,first_trg,count_s,gc_srctrg

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
             first_trg=trgphrase[1] ""
             gc_srctrg=c_srctrg[1]
             for(n=2;n<=num_trg_phrases;++n)
             {
               if(first_trg==trgphrase[n])
               {
                gc_srctrg+=c_srctrg[n]
               }
               else
               {
                if(first_src=="") printf"%s %d %d\n",first_trg,count_s,gc_srctrg
                else printf"%s %s %d %d\n",first_src,first_trg,count_s,gc_srctrg
                first_trg=trgphrase[n] ""
                gc_srctrg=c_srctrg[n]
               }
             }
             if(first_src=="") printf"%s %d %d\n",first_trg,count_s,gc_srctrg
             else printf"%s %s %d %d\n",first_src,first_trg,count_s,gc_srctrg
          }' $1
fi
