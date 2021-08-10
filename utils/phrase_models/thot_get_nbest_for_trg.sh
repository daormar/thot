# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Gets the first n source phrases for each target phrase with higher
# value of c_st. If n is between 0 and 1, then n is used as a threshold
# value. Given a target phrase t~, the script will show those
# translations whose probability is higher than n*max_prob, where
# max_prob is the maximum probability of all possible translations for
# t~
# NOTE: here, source phrases are those that appear in first place in
# the phrase table

filter_nbest()
{
    LC_NUMERIC="C" "${AWK}" -v n=${n_val} -v p=$p 'BEGIN{
                                count_option=0
                                remainder=0
                              }
                             {
                              i=1
                              while($i!="|||")
                              {
                               if(i==1) strn=$i
                               else strn=strn" "$i
                               ++i
                              }
                              if(count_option==0)
                              {
                               # handle first table entry
                               trgToProcess=strn
                               count_option=1
                               # keep the N-best
                               if(count_option<=n) print $0
                               else remainder+=$NF
                               } 
                              else
                              {
                               # handle following entries
                               if(trgToProcess==strn)
                               {
                                # process additional translation option
                                ++count_option
                                # keep the N-best
                                if(count_option<=n) print $0
                                else remainder+=$NF
                               }
                               else
                               {
                                # process a new block of translation options
                                if(p && remainder>0)
                                  printf "%s ||| <UNUSED_WORD> ||| 0 %.8f\n",trgToProcess,remainder
                                trgToProcess=strn
                                count_option=1
                                remainder=0
                                # keep the N-best
                                if(count_option<=n) print $0
                                else remainder+=$NF
                               }
                              }
                             }'
}

if [ $# -lt 2 -o $# -gt 7 ]; then
    echo "Usage: thot_get_nbest_for_trg [-t <string>] -n <int> [-p]"
    echo "                              [-T <string>]"
    echo ""
    echo "-t <string>          : Thot translation table to be processed"
    echo "                       (can be read from standard input)."
    echo "-n <int>             : Maximum number of translation options for each target"
    echo "                       phrase."
    echo "-p                   : Print remainder of the joint count for each block of"
    echo "                       translation options."
    echo "-T <string>          : Set directory for temporaries (if not given, /tmp is" 
    echo "                       used)."
else
    t_given=0
    ttable_file=""
    n_given=0
    p=0
    tmpdir="/tmp"
    while [ $# -ne 0 ]; do
        case $1 in
            "-t") shift
                if [ $# -ne 0 ]; then
                    ttable_file=$1
                    t_given=1
                else
                    t_given=0
                fi
                ;;
            "-n") shift
                if [ $# -ne 0 ]; then
                    n_given=1
                    n_val=$1
                fi
                ;;
            "-p") 
                p=1
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

    if [ ${n_given} -eq 0 ]; then
        echo "Error: value for n not given!"
        exit 1
    fi
    "${bindir}"/thot_flip_phr "${ttable_file}" | "${bindir}"/thot_sort_ttable -T "$tmpdir" | \
        filter_nbest | "${bindir}"/thot_flip_phr
fi
