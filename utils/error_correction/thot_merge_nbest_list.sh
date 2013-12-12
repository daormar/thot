# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Merges two n-best list files.

if [ $# -lt 2 ]; then
    echo "Usage: thot_merge_nbest_list <nblfile1> <nblfile2>"
else
    file1=$1
    file2=$2

    cat $file1 $file2 | ${AWK} '{if($1!="#") {$1="-"; printf"%s\n",$0}}' | \
        LC_ALL="" LC_COLLATE=C ${SORT} | uniq | $AWK 'BEGIN{printf"#\n"}{printf"%s\n",$0}'
fi
