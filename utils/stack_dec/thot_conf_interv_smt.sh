# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Computes confidence intervals for the results obtained by a
# statistical machine translation system

# \textbf{Categ}: evaluation

#####################################################################
standard_eval()
{
    $bindir/thot_scorer -r $ref -t $test
}

#####################################################################
bootstrap_exper()
{
    # Initialize random seed
    r=$RANDOM

    # Generate test corpus
    cat $test | \
        $AWK -v seed=$r -v ref=$ref -v S=$S -v tmp=$TMP '
         BEGIN{
	       nr=0;
               while(getline<ref) refh[++nr]=$0; 
              }
              {
               testh[NR]=$0;
              }
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

    # Compute measure for test corpus
    a=`$bindir/thot_scorer -r $TMP/ref -t $TMP/test | head -1 | cut -d " " -f 2`

    # Output measure
    echo $a
}

#####################################################################
bootstrap_eval()
{
    # Perform bootstrap experiments
    RANDOM=$seed
    n=1
    while [ $n -lt $N ]; do
        bootstrap_exper >> $TMP/scef
        n=`expr $n + 1`
    done

    # Obtain mean and stdev
    mean=`cat $TMP/scef | $AWK '{sum+=$0;}END{printf "%f", sum/NR;}'`
    echo "Mean: $mean"
    stDev=`cat $TMP/scef | $AWK -v med=$mean '{aux=$0-med; stDev+=aux*aux;}END{printf "%f", sqrt(stDev/(NR-1));}'`
    echo "stDev: $stDev"
    echo ""

    # Obtain confidence intervals
    LC_ALL=C $SORT -n $TMP/scef > $TMP/scef.sorted;
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
    
    # Print results
    echo "Confidence intervals";
    echo "conf.int.95 = [ $lb95 , $ub95 ] $center95 +- $rad95"
    echo "conf.int.90 = [ $lb90 , $ub90 ] $center90 +- $rad90"
}

#####################################################################
if [ $# -ne 5 ]; then 
    echo "thot_conf_interv_smt <seed> <ref_file> <test_file> <S> <N>"; 
    echo ""
    echo "<seed>               Random seed"
    echo "<ref_file>           File with references"
    echo "<test_file>          File with system translations"
    echo "<S>                  Size of the randomly generated subsets"
    echo "<N>                  Number of subsets"
    exit 1; 
fi

# Initialize variables
export LC_ALL=C;

seed=$1    # Random seed
ref=$2;    # File with references
test=$3;   # File with results of system 1
S=$4       # Size of the randomly generated subsets
N=$5;      # Number of subsets

# Check existence of files
if [ ! -f $ref ]; then
    echo "Error: file with reference sentences does not exist"
    exit 1
fi

if [ ! -f $test ]; then
    echo "Error: file with test sentences does not exist"
    exit 1
fi

# Create directory for temporary files
TMP=`mktemp -d`
trap "rm -rf $TMP* 2>/dev/null" EXIT

# Print random seed
echo "Random seed: $seed"
echo;

# Output result of standard evaluation
echo "Standard evaluation:"
standard_eval
echo ""

# Output result of bootstrap evaluation [Effron93]
echo "Bootstrap evaluation:"
bootstrap_eval
echo ""
