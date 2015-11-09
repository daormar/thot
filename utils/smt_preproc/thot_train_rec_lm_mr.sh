# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Given a monolingual corpus, extracts n-gram counts using the
# map-reduce technique. This allows to estimate model from corpora of an
# arbitrary size.

sort_counts()
{
    # Set sort command options
    export LC_ALL=""
    export LC_COLLATE=C
    if test ${sortT} = "yes"; then
        SORT_TMP="-T $TMP"
    else
        SORT_TMP=""
    fi

    ${AWK} '{printf"%d %s\n",NF,$0}' |\
        ${SORT} ${SORT_TMP} -t " " ${sortpars} | ${AWK} '{for(i=2;i<=NF-1;++i)printf"%s ",$i; printf"%s\n",$NF}'
}

merge_ngram_counts()
{
    ${AWK} '\
     BEGIN{
           prev_ngram=""
          }
          {
           if(NF>=2)
           {
            # Obtain ngram
            ngram=""
            for(i=1;i<NF;++i)
              if(i==1) ngram=$i
              else ngram=ngram" "$i
            if(ngram==prev_ngram)
              count+=$NF
            else
            {
             if(prev_ngram!="")
               printf"%s %d\n",prev_ngram,count
             prev_ngram=ngram
             count=$NF
            }
           }
          }
       END{
           printf"%s %d\n",ngram,count
          }'
}

usage()
{
    echo "thot_train_rec_lm_mr -r <string> -n <int> -o <string>"
    echo "                     [-tdir <string>]"
    echo "                     [-debug] [--help]"
    echo ""
    echo "-r <string>        : Corpus file."
    echo "-n <int>           : Order of the n-grams."
    echo "-o <string>        : Prefix of output files."
    echo "-tdir <string>     : Directory for temporary files."
    echo "-debug             : After ending, do not delete temporary files"
    echo "                     (for debugging purposes)."
    echo "--help             : Display this help and exit."
}

set_tmp_dir()
{
    if [ -d ${tdir} ]; then
        # Create directory for temporary files
        TMP=$tdir/thot_train_rec_lm_mr.$$
        if [ ${debug} -eq 0 ]; then
            trap "rm -rf $TMP* 2>/dev/null" EXIT
        fi
        mkdir $TMP
    else
        echo "Error: temporary directory does not exist"
        return 1;
    fi
}

r_given=0
n_given=0
o_given=0
tdir="/tmp"
debug=0
chunk_size=20000

if [ $# -eq 0 ]; then
    usage
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
        "-r") shift
            if [ $# -ne 0 ]; then
                corpus=$1
                r_given=1
            else
                r_given=0
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                output_pref=$1
                o_given=1
            else
                o_given=0
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
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1
            else
                tdir="./"
            fi
            ;;
        "-debug") debug=1
            ;;
    esac
    shift
done

# verify parameters

if [ ${r_given} -eq 0 ]; then
    echo "Error: corpus file not given"
    exit 1
else
    if [ ! -f  "${corpus}" ]; then
        echo "Error: file ${corpus} with training sentences does not exist"
        exit 1
    fi
fi

if [ ${o_given} -eq 0 ]; then
    echo "Error: -o option not provided"
    exit 1
fi

if [ ${n_given} -eq 0 ]; then
    echo "Error: order of the n-grams not provided"
    exit 1
fi

# parameters are ok

# Set TMP directory
set_tmp_dir || exit 1

# Split corpus into chunks of fixed size
${SPLIT} -l ${chunk_size} $corpus $TMP/

# Process chunks
c=1
for i in $TMP/*; do
    
    ${bindir}/thot_train_rec_lm -r $i -n ${n_val} -o $i 2> $i.lm.log
    
    c=`expr $c + 1`
    
    rm $i

done

# Merge counts
cat $TMP/*.lm | sort_counts | merge_ngram_counts > ${output_pref}.lm
cat $TMP/*.log > ${output_pref}.lm.log
