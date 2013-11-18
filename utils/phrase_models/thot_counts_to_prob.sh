# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Given a thot phrase ttable obtained with -pc option, 
# converts the counts to probability values

if [ $# -ne 1 -a $# -ne 0 ]; then
    echo "Usage: thot_counts_to_prob <thot_ttable>"
else
 
    ${AWK}              'BEGIN{ 
                            num_lines=0
                            num_src_words=0
                            num_trg_words=0
                           }
                           {
                            ++num_lines
                            countTrg=1
                            if(NF>=6)
                            {
                             for(i=1;i<=NF-2;++i)
                             {
                               printf"%s ",$i
                             }
                             prob=$NF/$(NF-1)
                             if(prob<0.00000001) prob=0.00000001
                             printf"%.8f\n",prob
                            }
		           }' $1
		  
fi
