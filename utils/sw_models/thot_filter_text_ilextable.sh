# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Filters an incremental lexical table in plain text given a corpus

if [ $# -ne 4 -a $# -ne 5 ]; then
    echo "Usage: thot_filter_text_ilextable <svoc> <tvoc>"
    echo "                                  <scorpus> <tcorpus> [<text_ilextable>]"
else
    svoc=$1
    tvoc=$2
    scorpus=$3
    tcorpus=$4

    if [ $# -eq  5 ]; then
        table=$5
    fi

    "${AWK}"  -v svoc="$svoc" -v tvoc="$tvoc" -v scorpus="$scorpus" -v tcorpus="$tcorpus" \
                 'BEGIN{ 
                        # Read source vocabulary
                        while( (getline <svoc) > 0)
                        {
                         sidx_to_word[$1]=$2
                        }
                        # Read target vocabulary
                        while( (getline <tvoc) > 0)
                        {
                         tidx_to_word[$1]=$2
                        }
                        # Read source and target corpora
                        while( (getline <scorpus) > 0)
                        {
                         # Store source sentence info
                         for(j=1;j<=NF;++j)
                         {
                           swords[j]=$j
                         }
                         slen=NF

                         # Read target corpus line
                         getline <tcorpus
                         # Store target sentence info
                         for(j=1;j<=NF;++j)
                         {
                           twords[j]=$j
                         }
                         tlen=NF

                         # Store word pairs
                         for(i=0;i<=slen;++i)
                          for(j=1;j<=tlen;++j)
                          {
                           if(i==0) sword="NULL"
                           else sword=swords[i]
                           pair=sword" "twords[j]
                           word_pairs[pair]=1
                          }
                        }
                       }
                       {
                        wpair=sidx_to_word[$1]" "tidx_to_word[$2]
                        if(wpair in word_pairs) printf"%s\n",$0 
                       }' "$table" 
fi
