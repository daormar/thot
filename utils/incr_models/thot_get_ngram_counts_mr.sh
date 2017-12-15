# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Given a monolingual corpus, extracts n-gram counts using the
# map-reduce technique. This allows to estimate model from corpora of an
# arbitrary size.  The -unk option reserves a certain probability mass
# for the unknown word.

disabled_pipe_fail()
{
    return $?
}

pipe_fail()
{
    # test if there is at least one command to exit with a non-zero status
    for pipe_status_elem in ${PIPESTATUS[*]}; do 
        if test ${pipe_status_elem} -ne 0; then 
            return 1; 
        fi 
    done
    return 0
}

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

sort_counts()
{
    # Set sort command options
    if test ${sortT} = "yes"; then
        SORT_TMP="-T $TMP"
    else
        SORT_TMP=""
    fi

    ${AWK} '{printf"%d %s\n",NF,$0}' | \
        LC_ALL=C ${SORT} ${SORT_TMP} -t " " ${sortpars} | \
        ${AWK} '{for(i=2;i<=NF-1;++i)printf"%s ",$i; printf"%d\n",$NF}' ; ${PIPE_FAIL} || return 1
}

print_desc()
{
    echo "thot_get_ngram_counts_mr written by Daniel Ortiz"
    echo "thot_get_ngram_counts_mr extracts n-grams counts from a monolingual corpus"
    echo "type \"thot_get_ngram_counts_mr --help\" to get usage information"
}

version()
{
    echo "thot_get_ngram_counts_mr is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

usage()
{
    echo "thot_get_ngram_counts_mr -c <string> -n <int> [-unk] [-f <int>]"
    echo "                          [-tdir <string>]"
    echo "                          [-debug] [--help] [--version]"
    echo ""
    echo "-c <string>        : Corpus file."
    echo "-n <int>           : Order of the n-grams."
    echo "-f <int>           : Size in lines of the fragments in which the corpus"
    echo "                     is divided when performing map-reduce (50K by default)."
    echo "-unk               : Reserve probability mass for the unknown word."
    echo "-tdir <string>     : Directory for temporary files."
    echo "-debug             : After ending, do not delete temporary files"
    echo "                     (for debugging purposes)."
    echo "--help             : Display this help and exit."
    echo "--version          : Output version information and exit."
}

set_tmp_dir()
{
    if [ -d ${tdir} ]; then
        # Create directory for temporary files
        TMP=$tdir/thot_get_ngram_counts_mr_${PPID}_$$
        if [ ${debug} -eq 0 ]; then
            trap "rm -rf $TMP* 2>/dev/null" EXIT
        fi
        mkdir $TMP
    else
        echo "Error: temporary directory does not exist"
        return 1;
    fi
}

unk_given=0
c_given=0
n_given=0
f_given=0
fragm_size=50000
tdir="/tmp"
debug=0
fragm_size=50000

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
        "-f") shift
            if [ $# -ne 0 ]; then
                fragm_size=$1
                f_given=1
            else
                f_given=0
            fi
            ;;
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1
            else
                tdir="./"
            fi
            ;;
        "-unk") unk_given=1
            ;;
        "-debug") debug=1
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

# Set TMP directory
set_tmp_dir || exit 1

# Process -unk option
if [ ${unk_given} -eq 1 ]; then
    TMPF_PCORPUS=`${MKTEMP} $tdir/pcorpus.XXXXXX`
    trap "rm ${TMPF_PCORPUS} ${TMPF_HIST_INFO} 2>/dev/null" EXIT

    cat $corpus | replace_first_word_occurrence_by_unk > ${TMPF_PCORPUS}
    proc_corpus=${TMPF_PCORPUS}
else
    proc_corpus=$corpus
fi

# Split corpus into fragments of fixed size
${SPLIT} -a 4 -l ${fragm_size} $proc_corpus $TMP/

# Process fragments
c=1
for i in `ls $TMP`; do
    
    ${bindir}/thot_get_ngram_counts -c $TMP/$i -n ${n_val} | \
        ${AWK} -v c=$c '{printf"%s %d\n",$0,c}' >> $TMP/counts ; ${PIPE_FAIL} || exit 1
    
    c=`expr $c + 1`
    
    rm $TMP/$i
done

# Merge counts
cat $TMP/counts | sort_counts | ${bindir}/thot_merge_ngram_counts ; ${PIPE_FAIL} || exit 1
