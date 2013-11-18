# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Given a monolingual corpus, obtains its corresponding vocabulary for
# its use in single word alignment models: <word_n> <word_index>
#
# If it is required, a list of special symbols can be given

if [ $# -ne 1 -a $# -ne 2 ]; then
    echo "Usage: thot_get_swm_vocab <text-file-path> [\"spsym_1 spsym_2 ... spsym_n\"]"
else

if [ $# -eq 1 ]; then
   sps=""
else
   sps="$2"
fi

${AWK} -v sps="${sps}"\
     'BEGIN{ 
            id=0
            if(sps!="")
            {
             numw=split(sps,words," ")
             for(i=1;i<=numw;++i)
             {
           #   printf words[i]
              vocab[words[i]]=id
              ++id
             }
            }
           }            
           {
	    for(i=1;i<=NF;++i)
            {
	     if(!($i in vocab)) 
             {
              freq[$i]=1
              vocab[$i]=id
              ++id
             } 
             else
             {
              ++freq[$i]
             }
            }
	   }
        END{ 
	    for(i in vocab)
             printf"%d %s %d\n",vocab[i],i,freq[i]
	   }' $1
fi
