# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Sorts a given thot ttable in order to binarize it.

########
put_st_lengths()
{
    # Puts the source and target phrase counts between the phrase pairs
    "${AWK}"              'BEGIN{ 
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
		           }' "$1"
}

########
remove_st_lengths()
{
    # Remove the lengths of s and t that have been previously 
    # put using put_st_lengths

    "${AWK}" '{       
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
      }' "${file}"
}

########
if [ $# -ne 0 -a $# -ne 2 -a $# -ne 4 ]; then
    echo "Usage: thot_sort_ttable [-t <thot_ttable(can be read from stdin)>]"
    echo "                        [-T <tmpdir>]"
else

    t_given=0
    ttable_file=""
    tmpdir="/tmp"
    while [ $# -ne 0 ]; do
        case $1 in
            "-t") shift
                if [ $# -ne 0 ]; then
                    ttable_file=$1
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

    if test "${sortT}" = "yes"; then
        SORT_TMP="$tmpdir"
    else
        SORT_TMP="/tmp"
    fi

    put_st_lengths "${ttable_file}" | LC_ALL=C "${SORT}" -T "${SORT_TMP}" ${sortpars} | remove_st_lengths
fi
