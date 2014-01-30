# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Extract n-gram counts from a monolingual corpus.

replace_first_word_occurrence_by_unk()
{
    ${AWK} '{
             for(i=1;i<=NF;++i)
             {
              if($i in vocab)
              {
               printf"%s",$i
              }
              else
              {
               vocab[$i]=1
               printf"<unk>"
              }
              if(i!=NF) printf" "
             }
             printf"\n" 
            }' 
}

get_counts()
{
    local order=$1
    ${AWK} -v n=$order\
                 'BEGIN{
                        num_words=0
                        num_sents=0
                       }
                       {
                        ++num_sents
                        num_words+=NF+2
                        #####################
                        for(i=1;i<=n;++i)
                        {
                          rightmost=NF+1
                          if(i==1) rightmost=NF
                          j=i-1
                          if(i==1) j=1
                          for(;j<=rightmost;++j)
                          {
                           cad=""
                           z=0
                           if(j-i+1>=0) z=j-i+1

                           for(;z<=j;++z)
                           {
                            if(z==0)
                            {
                             cad="<s>"
                             first=0
                            }
                            else
                            {
                             if(z==NF+1)
                             {
                               cad=cad" </s>"
                             }
                             else
                             {
                               if(cad=="") cad=$z
                               else cad=cad" "$z
                             }
                            }
                           }
                           if(!(cad in freq))
                           {
                            freq[cad]=1
                           }
                           else ++freq[cad]
                          }
                        }
                       }
                    END{
                        # Add eos and bos symbols
                        freq["<s>"]=num_sents;
                        freq["</s>"]=num_sents;

                        for(i in freq)
                        {
                          numw=split(i,words," ")
                          for(j=1;j<=numw;++j)
                          {
                             printf"%s ",words[j]
                          }
                          for(j=1;j<numw;++j)
                          {
                            if(j>1) cad=cad" "words[j]
                            else cad=words[j]
                          }
                          if(numw==1)
                          {
                           printf"%d %d\n",num_words,freq[i]
                          }
                          else
                          {
                           printf"%d %d\n",freq[cad],freq[i]
                          }
                        }
                       }'
}

print_desc()
{
    echo "thot_get_ngram_counts written by Daniel Ortiz"
    echo "thot_get_ngram_counts extracts n-grams counts from a monolingual corpus"
    echo "type \"thot_get_ngram_counts --help\" to get usage information"
}

version()
{
    echo "thot_get_ngram_counts is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

usage()
{
    echo "thot_get_ngram_counts -c <string> -n <int> [-unk]"
    echo "                      [-debug] [--help] [--version]"
    echo ""
    echo "-c <string>        : Corpus file."
    echo "-n <int>           : Order of the n-grams."
    echo "-unk               : Reserve probability mass for the unknown word."
    echo "--help             : Display this help and exit."
    echo "--version          : Output version information and exit."
}

unk_given=0
sdir=""
c_given=0
n_given=0

if [ $# -eq 0 ]; then
    print_desc
    exit 1
fi

while [ $# -ne 0 ]; do
    case $1 in
        "--help") usage
            exit 0
            ;;
        "--version") version
            exit 0
            ;;
        "-c") shift
            if [ $# -ne 0 ]; then
                corpus=$1
                c_given=1
            else
                c_given=0
            fi
            ;;
        "-n") shift
            if [ $# -ne 0 ]; then
                n_val=$1
                n_given=1
            else
                n_given=0
            fi
            ;;
        "-unk") unk_given=1
            ;;
    esac
    shift
done

# verify parameters

if [ ${c_given} -eq 0 ]; then
    echo "Error: corpus file not given"
    exit 1
else
    if [ ! -f  "${corpus}" ]; then
        echo "Error: file ${corpus} with training sentences does not exist"
        exit 1
    fi
fi

if [ ${n_given} -eq 0 ]; then
    echo "Error: order of the n-grams not provided"
    exit 1
fi

# parameters are ok

if [ $unk_given -eq 0 ]; then
    cat $corpus | get_counts ${n_val}
else
    cat $corpus | replace_first_word_occurrence_by_unk | get_counts ${n_val}
fi
