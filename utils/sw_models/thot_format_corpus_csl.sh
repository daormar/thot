# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Given source and target corpus files, formats them into one file for
# being processed with calc_swm_lgprob

if [ $# -ne 2 ]; then
    echo "Usage: thot_format_corpus_csl <src_corpus> <trg_corpus>"
else
    "$AWK" -v tcorpus="$2" '{printf"%s ||| ",$0; getline <tcorpus ; printf"%s\n",$0}' "$1"
fi
