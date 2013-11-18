# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Adds lexical scores for the phrase pairs of a given phrase table. The
# lexical scores (inverse and direct) are calculated as the score for
# the viterbi alignment of IBM 1 model. score_phr_ibm needs the prefix
# of the giza files and the phrase table. The phrase table can be read
# from standard input. The utility does not work for tables generated
# using -pc option.

if [ $# -ne 1 -a $# -ne 2 ]; then
    echo "Usage: thot_score_phr_ibm <giza_prefix> [<phrase-ttable(can be read from stdin)>]"
    echo "Note: this utility does not work for tables generated using the -pc option"
else

    if [ $# -eq 1 ]; then 
        ttable=""
    else
        ttable=$2
    fi

    smooth="0.001"

    gizapref=$1 
    LC_NUMERIC="C" ${AWK} -v gizapref=$1 -v smooth="${smooth}" \
                 '
                  function vit_alig(srcarray,srcasize,trgarray,trgasize)
                  {
                    for (i in alig) delete alig[i]
                    aligprob=1;
                    for(j=1;j<=trgasize;++j)
                    {
                     max_p=smooth
                     for(i=0;i<=srcasize;++i)
                     {
                      if(i==0) p=lex[0,trgvoc[trgarray[j]]]; 
                      else p=lex[srcvoc[srcarray[i]],trgvoc[trgarray[j]]];
#                      printf"%s %s %g\n",srcarray[i],trgarray[j],p
                      if(max_p<=p)
                      {
                       max_p=p; best_i=i;
                      }
                     }
                     aligprob=aligprob*max_p;
                     alig[j]=best_i
                    }
                    #if(aligprob<smooth) aligprob=smooth
                    return aligprob
                  }
                  function ivit_alig(srcarray,srcasize,trgarray,trgasize)
                  {
                    for (i in alig) delete alig[i]
                    aligprob=1;
                    for(j=1;j<=trgasize;++j)
                    {
                     max_p=smooth
                     for(i=0;i<=srcasize;++i)
                     {
                      if(i==0) p=ilex[srcvoc[trgarray[j]],0]; 
                      else p=ilex[trgvoc[srcarray[i]],srcvoc[trgarray[j]]];
#                      printf"%s %s %g\n",srcarray[i],trgarray[j],p
                      if(max_p<=p)
                      {
                       max_p=p; best_i=i;
                      }
                     }
                     aligprob=aligprob*max_p;
                     alig[j]=best_i
                    }
                    #if(aligprob<smooth) aligprob=smooth
                    return aligprob
                  }
                  BEGIN{
                        # read vocabularies and translation tables
                        srcvocfile=gizapref".trn.src.vcb"
                        while( (getline <srcvocfile) > 0)
                        {
                          srcvoc[$2]=$1
                        }
                        trgvocfile=gizapref".trn.trg.vcb"
                        while( (getline <trgvocfile) > 0)
                        {
                          trgvoc[$2]=$1
                        }
                        lexfile=gizapref".t3.final"
                        while( (getline <lexfile) > 0)
                        {
                          if($3<=smooth) lex[$1,$2]=smooth
                          else lex[$1,$2]=$3
                        }
                        ilexfile=gizapref".ti.final"
                        while( (getline <ilexfile) > 0)
                        {
                          if($3<=smooth) ilex[$1,$2]=smooth
                          else ilex[$1,$2]=$3
                        }
                       }
                       {
                        # extract phrase1 phrase
                        num_phrase1_words=1
                        phrase1=$1 ""
                        phrase1array[1]=$1
                        for(i=2;i<=NF;++i)
                        {
                         if($i != "|||")
                         {
                          ++num_phrase1_words
                          phrase1=phrase1" "$i
                          phrase1array[i]=$i
                         }
                         else
                         {
                          break
                         }
                        }
                        # extract phrase2 phrase
                        num_phrase2_words=1
                        ++i; phrase2=$i; 
                        phrase2array[1]=$i
                        ++i;
                        for(;i<=NF;++i)
                        {
                         if($i != "|||")
                         {
                          ++num_phrase2_words
                          phrase2=phrase2" "$i
                          phrase2array[i-num_phrase1_words-1]=$i
                         }
                         else
                         {
                          break
                         }
                        }
                        printf"%s %g %g\n",$0,vit_alig(phrase2array,num_phrase2_words,phrase1array,num_phrase1_words),ivit_alig(phrase1array,num_phrase1_words,phrase2array,num_phrase2_words)
                       }' ${ttable} 
fi
