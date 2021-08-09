# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Generates an initial file with weights for io language models.  The
# order of the lm, the number of buckets and their size have to be
# provided. Additionally, a given weight value can be provided to
# generate the file.

if [ $# -ne 3 -a $# -ne 4 ]; then
    echo "Usage: thot_gen_init_file_with_jmlm_weights <order> <numbuckets> <bucketsize> [wval]"
else
    order=$1
    numbuckets=$2
    bucketsize=$3

    if [ $# -eq 4 ]; then
        wval=$4
    else
        wval=0.5
    fi

    echo "" | "$AWK" -v order=$order -v numbuckets=$numbuckets -v bucketsize=$bucketsize -v wval=$wval '
                 {
                  printf"%d %d %d",order,numbuckets,bucketsize
                  for(i=1;i<=order*numbuckets;++i)
                    printf" %s",wval
                  printf"\n"
                 }'
fi
