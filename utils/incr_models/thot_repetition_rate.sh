# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Given a monolingual corpus file, calculates its repetition rate.

obtain_statistics()
{
    # Get parameters
    file=$1

    # Calculate total number of n-grams for n=1...4
    total1=`cat $file | $AWK '{if(NF==3) num=num+$NF}END{printf"%d\n",num}'`
    total2=`cat $file | $AWK '{if(NF==4) num=num+$NF}END{printf"%d\n",num}'`
    total3=`cat $file | $AWK '{if(NF==5) num=num+$NF}END{printf"%d\n",num}'`
    total4=`cat $file | $AWK '{if(NF==6) num=num+$NF}END{printf"%d\n",num}'`

    # Calculate number of distinct n-grams for n=1...4
    dist1=`cat $file | $AWK '{if(NF==3) ++num}END{printf"%d\n",num}'`
    dist2=`cat $file | $AWK '{if(NF==4) ++num}END{printf"%d\n",num}'`
    dist3=`cat $file | $AWK '{if(NF==5) ++num}END{printf"%d\n",num}'`
    dist4=`cat $file | $AWK '{if(NF==6) ++num}END{printf"%d\n",num}'`

    # Calculate number of singleton n-grams for n=1...4
    sing1=`cat $file | $AWK '{if(NF==3 && $NF==1) ++num}END{printf"%d\n",num}'`
    sing2=`cat $file | $AWK '{if(NF==4 && $NF==1) ++num}END{printf"%d\n",num}'`
    sing3=`cat $file | $AWK '{if(NF==5 && $NF==1) ++num}END{printf"%d\n",num}'`
    sing4=`cat $file | $AWK '{if(NF==6 && $NF==1) ++num}END{printf"%d\n",num}'`

    # Calculate rates for n=1...4
    rate1=`echo "" | $AWK -v dist=$dist1 -v sing=$sing1 '{if(dist==0) printf"0\n"; else printf "%f\n",(dist-sing)/dist }'`
    rate2=`echo "" | $AWK -v dist=$dist2 -v sing=$sing2 '{if(dist==0) printf"0\n"; else printf "%f\n",(dist-sing)/dist }'`
    rate3=`echo "" | $AWK -v dist=$dist3 -v sing=$sing3 '{if(dist==0) printf"0\n"; else printf "%f\n",(dist-sing)/dist }'`
    rate4=`echo "" | $AWK -v dist=$dist4 -v sing=$sing4 '{if(dist==0) printf"0\n"; else printf "%f\n",(dist-sing)/dist }'`
}

print_statistics()
{
    string=$1
    echo "$string ; statistics for n-grams (n=1...4): Totals: $total1/$total2/$total3/$total4 ; Distinct: $dist1/$dist2/$dist3/$dist4 ; Singletons: $sing1/$sing2/$sing3/$sing4 ; Rates: $rate1/$rate2/$rate3/$rate4"
}

compute_rr()
{
    echo "$rate1 $rate2 $rate3 $rate4" | $AWK '{zero_present=0; for(i=1;i<=NF;++i) if($i==0) zero_present=1; if(zero_present) rr=0; else {prod=$1*$2*$3*$4; rr=exp((1/4)*log(prod));} printf"%.3f\n",rr}'
}

transform_count_file()
{
    # Obtain input parameters
    file=$1
    label=$2

    # Transform file
    cat $file | $AWK -v label=$label '{printf"%s %s %s",label,$(NF-1),$NF; for(i=1;i<=NF-2;++i) printf" %s",$i; printf"\n"}'
}

obtain_ngc_of_corpus_not_present_in_train()
{
    # Obtain input parameters
    corpus_ngc=$1
    train_ngc=$2
    
    cat $corpus_ngc $train_ngc | LC_ALL=C $SORT -k 4 | $UNIQ -f 3 -u | $GREP "^corpus" | $AWK '{for(i=4;i<=NF;++i) printf"%s ",$i; printf"%s %s\n",$2,$3}'
}

compute_unf_for_n()
{
    ifrac1=`echo "$cdist1 $diff_dist1" | $AWK '{printf "%f\n",$2/$1 }'`
    ifrac2=`echo "$cdist2 $diff_dist2" | $AWK '{printf "%f\n",$2/$1 }'`
    ifrac3=`echo "$cdist3 $diff_dist3" | $AWK '{printf "%f\n",$2/$1 }'`
    ifrac4=`echo "$cdist4 $diff_dist4" | $AWK '{printf "%f\n",$2/$1 }'`
}

compute_unf_for_n_totals()
{
    ifrac1=`echo "$ctotal1 $diff_total1" | $AWK '{printf "%f\n",$2/$1 }'`
    ifrac2=`echo "$ctotal2 $diff_total2" | $AWK '{printf "%f\n",$2/$1 }'`
    ifrac3=`echo "$ctotal3 $diff_total3" | $AWK '{printf "%f\n",$2/$1 }'`
    ifrac4=`echo "$ctotal4 $diff_total4" | $AWK '{printf "%f\n",$2/$1 }'`
}

compute_unf()
{
    echo "$ifrac1 $ifrac2 $ifrac3 $ifrac4" | $AWK '{zero_present=0; for(i=1;i<=NF;++i) if($i==0) zero_present=1; if(zero_present) rr=0; else {prod=$1*$2*$3*$4; rr=exp((1/4)*log(prod))} printf"%.3f\n",rr}'
}

print_desc()
{
    echo "thot_repetition_rate written by Daniel Ortiz"
    echo "thot_repetition_rate obtains repetition rate measures for a given corpus"
    echo "type \"thot_repetition_rate --help\" to get usage information"
}

usage()
{
    echo "thot_repetition_rate -c <string> [-t <string>]"
    echo "                     [--help] [--version]"
    echo ""
    echo "-c <string>        : Corpus file."
    echo "-t <string>        : Training corpus."
    echo "--help             : Display this help and exit."
    echo "--version          : Output version information and exit."
}

version()
{
    echo "thot_repetition_rate is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}


c_given=0
t_given=0

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
        "-t") shift
            if [ $# -ne 0 ]; then
                tcorpus=$1
                t_given=1
            else
                t_given=0
            fi
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
        echo "Error: file ${corpus} does not exist"
        exit 1
    fi
fi

if [ ${t_given} -eq 1 ]; then
    if [ ! -f  "${tcorpus}" ]; then
        echo "Error: file ${tcorpus} with training sentences does not exist"
        exit 1
    fi
fi

# parameters are ok

# Create temporary files
tmpdir=/tmp

tmpfile=`${MKTEMP} $tmpdir/tmpfile.XXXXXX`

if [ ${t_given} -eq 1 ]; then
    tmpfile2=`${MKTEMP} $tmpdir/tmpfile.XXXXXX`
    tmpfile3=`${MKTEMP} $tmpdir/tmpfile.XXXXXX`
    tmpfile4=`${MKTEMP} $tmpdir/tmpfile.XXXXXX`
    tmpfile5=`${MKTEMP} $tmpdir/tmpfile.XXXXXX`
fi

trap "rm $tmpfile $tmpfile2 $tmpfile3 $tmpfile4 $tmpfile5" EXIT

# Obtain file with counts
$bindir/thot_get_ngram_counts_mr -c $corpus -n 4 > $tmpfile

# Obtain statistics
obtain_statistics $tmpfile

# Print statistics
print_statistics $corpus

# Save dist statistics
cdist1=$dist1
cdist2=$dist2
cdist3=$dist3
cdist4=$dist4

# Save total statistics
ctotal1=$total1
ctotal2=$total2
ctotal3=$total3
ctotal4=$total4

# Compute repetition rate
rr=`compute_rr $rate1 $rate2 $rate3 $rate4`
    
# Print result
echo "* RR($corpus)= $rr"

# Process training corpus if given
if [ ${t_given} -eq 1 ]; then
    # Obtain file with counts
    $bindir/thot_get_ngram_counts_mr -c $tcorpus -n 4 > $tmpfile2

    # Obtain statistics
    obtain_statistics $tmpfile2

    # Transform count files
    transform_count_file $tmpfile "corpus" > $tmpfile3
    transform_count_file $tmpfile2 "train" > $tmpfile4

    # Obtain n-grams counts of corpus not present in train
    obtain_ngc_of_corpus_not_present_in_train $tmpfile3 $tmpfile4 > $tmpfile5

    # Obtain statistics
    obtain_statistics $tmpfile5

    # Save dist statistics
    diff_dist1=$dist1
    diff_dist2=$dist2
    diff_dist3=$dist3
    diff_dist4=$dist4

    # Save total statistics
    diff_total1=$total1
    diff_total2=$total2
    diff_total3=$total3
    diff_total4=$total4

    # Compute unseen n-gram fraction for each value of n
    # compute_unf_for_n
    compute_unf_for_n_totals

    # Compute unseen n-gram fraction
    ifrac=`compute_unf`

    # Print unseen n-gram fraction
    echo "* UNF($corpus,$tcorpus)= $ifrac ; $ifrac1/$ifrac2/$ifrac3/$ifrac3"

    # Print statistics
    print_statistics "$corpus , $tcorpus"

    # Compute repetition rate
    rr=`compute_rr $rate1 $rate2 $rate3 $rate4`
    
    # Print result
    echo "* RRR($corpus,$tcorpus)= $rr"
fi
