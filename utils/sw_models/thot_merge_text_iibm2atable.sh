# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Merges a set of alignment counts generated during the estimation of an
# incremental HMM alignment model using map reduce.

merge_sorted_counts()
{
    ${AWK} -v n_val=${n_val} '\
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
       first_trg=trgparam[1]
       glc_srctrg=lc_srctrg[1]
       for(n=2;n<=num_trg_params;++n)
       {
         if(first_trg==trgparam[n])
         {
          # Accumulate count of target phrase for additional chunk
          glc_srctrg=lns_sumlog(glc_srctrg,lc_srctrg[n])
         }
         else
         {
          # Print count for current target phrase
          printf"%s %s %g %g\n",first_src,first_trg,glc_srctrg,lc_src
 
          # Initialize variables for next target phrase
          first_trg=trgparam[n]
          glc_srctrg=lc_srctrg[n]
         }
       }
       # Print last target phrase
       printf"%s %s %g %g\n",first_src,first_trg,glc_srctrg,lc_src
     }
     BEGIN{
           lc_src=-99
           first_entry=1
          }
          {
           # extract source data
           src=$1" "$2" "$3

           # extract target data
           trg=$4

           # extract joint count
           lc_st=$(NF-2)

           # verify if it is the first entry of the table
           if(first_entry==1)
           {
             first_src=src
             first_entry=0
             num_trg_params=1
             trgparam[num_trg_params]=trg
             lc_srctrg[num_trg_params]=lc_st
           }
           else
           {
             # a new source parameter has appeared?
             if(first_src!=src)
             {
               # print counts
               print_counts()

               # reset variables
               first_src=src
               delete_array(chunks)
               delete_array(trgparam) 
               delete_array(lc_srctrg) 
               num_trg_params=1
               trgparam[num_trg_params]=trg
               lc_srctrg[num_trg_params]=lc_st
               lc_src=-99
             }
             else
             {
               ++num_trg_params
               trgparam[num_trg_params]=trg
               lc_srctrg[num_trg_params]=lc_st
             }
           }
           if(!($NF in chunks)) 
           {
             chunks[$NF]=$(NF-1)
             lc_src=lns_sumlog(lc_src,$(NF-1))
           }
          }
       END{
             # print last group of counts
             print_counts()
          }'
}

if [ $# -ne 2 -a $# -ne 3 -a $# -ne 4 -a $# -ne 5 ]; then
    echo "Usage: thot_merge_text_iibm2atable [-t <text_ilextable>] [-ns] [-T <tmpdir>]"
else
    # Take parameters
    t_given=0
    ttable_file=""
    ns_given=0
    tmpdir="/tmp"
    while [ $# -ne 0 ]; do
        case $1 in
            "-t") shift
                if [ $# -ne 0 ]; then
                    table=$1
                    t_given=1
                fi
                ;;
            "-ns") ns_given=1
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

    # Merge table counts
    if [ ${ns_given} -eq 1 ]; then
        cat $table | merge_sorted_counts
    else
        cat $table | LC_ALL=C ${SORT} ${SORT_TMP} ${sortpars} -k1n -k2n -k3n -k4n | merge_sorted_counts
    fi
fi
