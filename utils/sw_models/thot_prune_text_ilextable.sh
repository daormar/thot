# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Prunes an incremental lexical table given in plain text by keeping the
# n best translation for each word

function prune_sorted_table()
{
 ${AWK} -v n_val=${n_val} -v c_val=${c_val} '\
     function delete_array(array)
     {
      delete array
     }
     function lns_sumlog(logx,logy)
     {
      if(lx>ly)
      {
       return logx+log(1+exp(logy-logx));
      }
      else
      {
       return logy+log(1+exp(logx-logy));
      }
     }
     function print_counts()
     {
       # Determine number of counts to print
       new_lc_src=-99999
       num_filt_trg_words=0
       for(n=1;n<=num_trg_words;++n)
       {
         prob=exp(lc_srctrg[n]-lc_src)
         if((num_filt_trg_words<n_val || n_val<=0) && prob>=c_val) 
         {
          new_lc_src=lns_sumlog(new_lc_src,lc_srctrg[n]);
          ++num_filt_trg_words
         }
         else break
       }
       # Print counts
       for(n=1;n<=num_filt_trg_words;++n)
       {
         printf"%s %s %g %g\n",first_src,trgword[n],lc_srctrg[n],new_lc_src
       }
     }
     BEGIN{
           first_entry=1
          }
          {
           # extract source word
           src=$1

           # extract target word
           trg=$2

           # extract joint count
           lc_st=$(NF-1)

           # verify if it is the first entry of the table
           if(first_entry==1)
           {
             first_src=src
             first_entry=0
             num_trg_words=1
             trgword[num_trg_words]=trg
             lc_srctrg[num_trg_words]=lc_st
             lc_src=lc_st
           }
           else
           {
             # a new source word has appeared?
             if(first_src!=src)
             {
               # print counts
               print_counts()

               # reset variables
               first_src=src
               delete_array(trgword) 
               delete_array(lc_srctrg) 
               num_trg_words=1
               trgword[num_trg_words]=trg
               lc_srctrg[num_trg_words]=lc_st
               lc_src=lc_st
             }
             else
             {
               ++num_trg_words
               trgword[num_trg_words]=trg
               lc_srctrg[num_trg_words]=lc_st
               lc_src=lns_sumlog(lc_src,lc_srctrg[num_trg_words]);
             }
           }
          }
       END{
             # print last group of counts
             print_counts()
          }'
}

if [ $# -ne 2 -a $# -ne 4 -a $# -ne 6 -a $# -ne 8 ]; then
    echo "Usage: thot_prune_text_ilextable -n <n_val> [-c <cut-off-val>] [-t <text_ilextable>]"
    echo "                                 [-T <tmpdir>]"

else

    # Take parameters
    n_given=0
    t_given=0
    c_given=0
    c_val=0
    ttable_file=""
    tmpdir="/tmp"
    while [ $# -ne 0 ]; do
        case $1 in
            "-n") shift
                if [ $# -ne 0 ]; then
                    n_val=$1
                    n_given=1
                fi
                ;;
            "-c") shift
                if [ $# -ne 0 ]; then
                    c_val=$1
                    c_given=1
                fi
                ;;
            "-t") shift
                if [ $# -ne 0 ]; then
                    table=$1
                    t_given=1
                fi
                ;;
            "-T") shift
                if [ $# -ne 0 ]; then
                    tmpdir=$1
                else
                    tmpdir="/tmp"
                fi
                ;;
        esac
        shift
    done

    if test ${sortT} = "yes"; then
        SORT_TMP="-T $tmpdir"
    else
        SORT_TMP=""
    fi

    # Check parameters
    if [ ${n_given} -eq 0 ]; then
        echo "Error: -n parameter not given"
        exit 1
    fi

    # Prune table
    cat $table | LC_ALL=C ${SORT} ${SORT_TMP} ${sortpars} -k1n -k3gr | prune_sorted_table
fi
