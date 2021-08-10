# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Filters those ttable entries which are to be accesed
# to translate a given test corpus

if [ $# -ne 1 -a $# -ne 2 ]; then
    echo "Usage: thot_filter_ttable_given_corpus [<thot_pc_ttable(can be read from stdin)>]"
    echo "                                       <test_corpus>"
else

    # Obtain and check parameters
    if [ $# -eq 2 ]; then
        file=$2
        input=$1
        if [ ! -f "$file" ]; then
            echo "Error: File \"$file\" does not exist" >&2
            exit 1
        fi
        if [ ! -f "$input" ]; then
            echo "Error: File \"$input\" does not exist" >&2
            exit 1
        fi
    else
        file=$1
        if [ ! -f "$file" ]; then
            echo "Error: File \"$file\" does not exist" >&2
            exit 1
        fi
    fi

    # Filter translation table
    "${AWK}" -v file="$file" \
       ' BEGIN { 
                 used=0
                 total=0
                 while( (getline <file) > 0)
                 {
                   for(i=1;i<=NF;i++) 
                   {
                     for(j=0;j<NF && j+i<=NF;j++) 
                     {
                        phrase = "";
                        for(k=i;k<=i+j;k++) 
                        {
                           if(k==i+j) phrase=phrase $k
                           else phrase=phrase $k" "
                        }
                        if(phrase in testCorpus)
                        {
                          ++testCorpus[phrase]
                        }
                        else
                        {
                          # print phrase
                          testCorpus[phrase]=1
                        }	
                     }
                   }
                 }
               }
               {
                 for(i=1;i<NF;++i)
                 {
                   if($i == "|||")
                   {
                     ++i
                     break;
                   }
                 }
                 phrase=""
                 for(;i<NF;++i)
                 {
                   if($i == "|||")
                   {
                     break;
                   }
                   else
                   {
                     if($(i+1)=="|||") phrase=phrase $i
                     else phrase=phrase $i" "
                   }
                 }
                 if(phrase in testCorpus) 
                 {
                   print $0;
                   ++used;
                 }
                 ++total;
               }
            END{
                 printf "%d of %d phrase pairs used (%.2f%%)\n",used,total,(100*used/total) > "/dev/stderr" 
               }' "$input"
fi
