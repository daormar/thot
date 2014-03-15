# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Computes confidence intervals for the results obtained by a
# statistical machine translation system

# \textbf{Categ}: evaluation

SCR=${0##*/};

if [ $# -ne 6 ]; then 
    echo "Computes significance tests"
    echo ""
    echo "$SCR <seed> <ref_file> <test_file> <S> <N> <measure>"; 
    echo "<seed>              Random seed"
    echo "<ref_file>          File with references"
    echo "<test_file>         File with results of system 1"
    echo "<S>                 Size of the randomly generated subsets"
    echo "<N>                 Number of subsets"
    echo "<measure>           Translation quality measure: BLEU | WER"
    exit 1; 
fi

export LC_ALL=C;

TMP=${TMPDIR:-/tmp}/$SCR.$$
trap "rm -rf $TMP* 2>/dev/null" EXIT
mkdir $TMP

seed=$1    # Random seed
ref=$2;    # File with references
test=$3;   # File with results of system 1
S=$4       # Size of the randomly generated subsets
N=$5;      # Number of subsets
measure=$6;# Translation quality measure

# Check existence of files
if [ ! -f $ref ]; then
    echo "Error: file with reference sentences does not exist"
    exit 1
fi

if [ ! -f $test ]; then
    echo "Error: file with test sentences does not exist"
    exit 1
fi

# Print random seed
echo "Random seed: $seed"
echo;

# Standard evaluation
echo "Standard evaluation:"
if [ $measure = "WER" ]; then
    $bindir/thot_calc_wer $ref $test
else
    $bindir/thot_calc_bleu -r $ref -t $test
fi
echo;

# Bootstrap Effron93
RANDOM=$seed
for ((n=1;n<=N;n++)); do
    r=$RANDOM
    cat $test | \
        $AWK -v seed=$r -v ref=$ref -v S=$S -v tmp=$TMP '
    BEGIN{
	  nr=0;
          while(getline<ref) refh[++nr]=$0; 
	 }
    {testh[NR]=$0;}
    END{
        srand(seed);
        for(i=1;i<=S;i++)
        {
         r=rand();
         id=1+int(NR*r);
         printf "%s\n",testh[id] > tmp"/test";
         printf "%s\n",refh[id] > tmp"/ref";
        }
       }'
    if [ $measure = "WER" ]; then
        a=`$bindir/thot_calc_wer $TMP/ref $TMP/test | head -1 | cut -d " " -f 3`
    else
        a=`$bindir/thot_calc_bleu -r $TMP/ref -t $TMP/test | head -1 | cut -d " " -f 2`
    fi

    echo $a >> $TMP/scef;
done

# Mean and stDev
mean=`cat $TMP/scef | $AWK '{sum+=$0;}END{printf "%f", sum/NR;}'`
echo "Mean: $mean"
stDev=`cat $TMP/scef | $AWK -v med=$mean '{aux=$0-med; stDev+=aux*aux;}END{printf "%f", sqrt(stDev/(NR-1));}'`
echo "stDev: $stDev"
echo;

# Confidence interval
sort -n $TMP/scef > $TMP/scef.sorted;
l95=`echo "" | $AWK -v N=$N '{printf"%d",0.025*N+1}' `
l90=`echo "" | $AWK -v N=$N '{printf"%d",0.05*N+1}' `;
s95=`echo "" | $AWK -v N=$N -v l95=${l95} '{printf"%d",N-l95+1}' `;
s90=`echo "" | $AWK -v N=$N -v l90=${l90} '{printf"%d",N-l90+1}' `;

lb95=`cat $TMP/scef.sorted | head -${l95}  | tail -1`;
ub95=`cat $TMP/scef.sorted | head -${s95} | tail -1`;
center95=`echo "" | $AWK -v l=$lb95 -v u=$ub95 '{printf"%f",l+(u-l)/2}'`
rad95=`echo "" | $AWK -v l=$lb95 -v u=$ub95 '{printf"%f",(u-l)/2}'`
lb90=`cat $TMP/scef.sorted | head -$l90  | tail -1`;
ub90=`cat $TMP/scef.sorted | head -$s90 | tail -1`;
center90=`echo "" | $AWK -v l=$lb90 -v u=$ub90 '{printf"%f",l+(u-l)/2}'`
rad90=`echo "" | $AWK -v l=$lb90 -v u=$ub90 '{printf"%f",(u-l)/2}'`

echo "Confidence intervals";
echo "conf.int.95 = [ $lb95 , $ub95 ] $center95 +- $rad95"
echo "conf.int.90 = [ $lb90 , $ub90 ] $center90 +- $rad90"
echo;
