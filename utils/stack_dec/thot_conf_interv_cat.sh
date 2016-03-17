# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Computes confidence intervals for the results obtained by a computer
# assisted translation system

# \textbf{Categ}: evaluation acl09

#####################################################################
standard_eval()
{
    $GREP "PARTIAL" $res | \
        $AWK 'BEGIN{
                    chars=0
                    ks=0
                    ksma=0
                   }
                   {
                    chars+=$6
                    ks+=$(10)
                    ksma+=$(17)
                   }
                END{
                    KSR=(ks/chars)*100
                    KSMR=(ksma/chars)*100
                    printf"%f\n",KSMR
                    printf"%f\n",KSR
                   }'
}

#####################################################################
bootstrap_exper()
{
    r=$RANDOM
    $GREP "PARTIAL" $res | \
        $AWK -v seed=$r -v S=$S -v measure=$measure -v tmp=$TMP '
             {
              resh[NR]=$0;
             }
          END{
              chars=0
              ks=0
              ksma=0
              srand(seed);
              for(i=1;i<=S;i++)
              {
               r=rand();
               id=1+int(NR*r);
               split(resh[id],fields)
               chars+=fields[6]
               ks+=fields[10]
               ksma+=fields[17]
              }
              KSR=(ks/chars)*100
              KSMR=(ksma/chars)*100
              if(measure=="KSMR") printf"%f\n",KSMR
              else printf"%f\n",KSR
             }'
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
    echo;

    # Obtain confidence intervals
    $SORT -n $TMP/scef > $TMP/scef.sorted;
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

    # Output results
    echo "Confidence intervals";
    echo "conf.int.95 = [ $lb95 , $ub95 ] $center95 +- $rad95"
    echo "conf.int.90 = [ $lb90 , $ub90 ] $center90 +- $rad90"
    echo ""
}

#####################################################################

if [ $# -ne 4 -a $# -ne 5 ]; then 
    echo "Computes significance tests"
    echo ""
    echo "Use: $SCR <seed> <cat_results_file> <S> <N> [measure]"; 
    echo ""
    echo "<seed>              Random seed"
    echo "<cat_results_file>  File with CAT results"
    echo "<S>                 Size of the randomly generated subsets"
    echo "<N>                 Number of subsets"
    echo "<measure>           KSR|KSMR (KSMR by default)"
    exit 1; 
fi

# Initialize variables
export LC_ALL=C;

seed=$1   # Random seed
res=$2;   # File with cat results
S=$3      # Size of the randomly generated subsets
N=$4;     # Number of subsets

if [ $# -eq 4 ]; then
    measure="KSMR"
else
    measure=$5
fi

# Check existence of files
if [ ! -f $res ]; then
    echo "Error: file with cat results does not exist"
    exit 1
fi

# Create directory for temporary files
TMP=`mktemp -d`
trap "rm -rf $TMP* 2>/dev/null" EXIT

# Print random seed
echo "Random seed: $seed"
echo ""

# Output result of standard evaluation
echo "Standard evaluation:"
standard_eval
echo ""

# Output result of bootstrap evaluation [Effron93]
echo "Bootstrap evaluation:"
bootstrap_eval
echo ""
