# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Given a corpus file representing a population, calculates the
# representativeness of a sample file.

#############
print_desc()
{
    echo "thot_corpus_represent written by Daniel Ortiz"
    echo "thot_corpus_represent obtains repetition rate measures for a given corpus"
    echo "type \"thot_corpus_represent --help\" to get usage information"
}

#############
usage()
{
    echo "thot_corpus_represent -p <string> -s <string> [-u <string>] [-w]"
    echo "                      [--help] [--version]"
    echo ""
    echo "-p <string>        : Population file."
    echo "-s <string>        : Sample file."
    echo "-u <string>        : File with unrelated text (size will be adjusted)."
    echo "-w                 : Use words instead of lines to calculate sample size."
    echo "--help             : Display this help and exit."
    echo "--version          : Output version information and exit."
    echo ""
}

#############
version()
{
    echo "thot_corpus_represent is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

#############
numwords_to_numlines()
{
    local_numw=$1
    local_file=$2

    $AWK -v numw=${local_numw} \
         'BEGIN{
           numl=-1
          }
          {
           if(numl==-1 && sum>numw)
           {
            numl=NR
           }
           sum+=NF
          }
          END{
           if(numl==-1) numl=NR
           printf "%d",numl
          }' ${local_file}
       
}

#############
calc_ln_pop_sample()
{
    local_sample=$1
    local_pop=$2
    
    if [ ${w_given} -eq 0 ]; then
        # Obtain number of lines of sample
        local_sample_lnum=`$WC ${local_sample} | $AWK '{printf"%s",$1}'`
        
        echo ${local_sample_lnum}
    else
        # Obtain number of words of sample
        local_sample_wnum=`$WC ${local_sample} | $AWK '{printf"%s",$2}'`
        
        # Determine number of lines to be extracted
        local_sample_lnum=`numwords_to_numlines ${local_sample_wnum} ${local_pop}`

        echo ${local_sample_lnum}
    fi
}
 
#############

p_given=0
s_given=0
u_given=0
w_given=0

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
        "-p") shift
            if [ $# -ne 0 ]; then
                population=$1
                p_given=1
            else
                s_given=0
            fi
            ;;
        "-s") shift
            if [ $# -ne 0 ]; then
                sample=$1
                s_given=1
            else
                s_given=0
            fi
            ;;
        "-u") shift
            if [ $# -ne 0 ]; then
                unrelated=$1
                u_given=1
            else
                u_given=0
            fi
            ;;
        "-w") w_given=1
              ;;
    esac
    shift
done

# verify parameters
if [ ${p_given} -eq 0 ]; then
    echo "Error: -p option not given"
    exit 1
else
    if [ ! -f  "${population}" ]; then
        echo "Error: file ${corpus} does not exist"
        exit 1
    fi
fi

if [ ${s_given} -eq 0 ]; then
    echo "Error: -s option not given"
    exit 1
else
    if [ ! -f  "${sample}" ]; then
        echo "Error: file ${sample} does not exist"
        exit 1
    fi
fi

if [ ${u_given} -eq 1 -a ! -f  "${unrelated}" ]; then
    echo "Error: file ${unrelated} does not exist"
    exit 1
fi

# parameters are ok

# Create temporary files
tmpdir=/tmp

shuff_tmpfile=`${MKTEMP} $tmpdir/shuff_tmpfile.XXXXXX`
population_tmpfile=`${MKTEMP} $tmpdir/population_tmpfile.XXXXXX`
popsample_tmpfile=`${MKTEMP} $tmpdir/popsample_tmpfile.XXXXXX`
unrelated_tmpfile=`${MKTEMP} $tmpdir/unrelated_tmpfile.XXXXXX`

trap "rm $shuff_tmpfile $population_tmpfile $popsample_tmpfile $unrelated_tmpfile" EXIT

# Shuffle population
${bindir}/thot_shuffle 31415 $tmpdir ${population} > ${shuff_tmpfile}

# Calculate number of lines to extract population sample
sample_lnum=`calc_ln_pop_sample $sample ${shuff_tmpfile}`

# Obtain partition of population
$HEAD -${sample_lnum} ${shuff_tmpfile} > ${popsample_tmpfile}
$TAIL -n +`expr ${sample_lnum} + 1` ${shuff_tmpfile} > ${population_tmpfile}

# Handle unrelated file if given
if [ ${u_given} -eq 1 ]; then
    unrel_lnum=`calc_ln_pop_sample $sample ${unrelated}`
    ${bindir}/thot_shuffle 31415 $tmpdir ${unrelated} | $HEAD -${unrel_lnum} > ${unrelated_tmpfile}
fi

# Compute repetition rates
unf_popsample=`${bindir}/thot_repetition_rate -c ${popsample_tmpfile} -t ${population_tmpfile} | $GREP UNF | $AWK '{printf"%s",$3}'`
unf_sample=`${bindir}/thot_repetition_rate -c ${sample} -t ${population_tmpfile} | $GREP UNF | $AWK '{printf"%s",$3}'`
if [ ${u_given} -eq 1 ]; then
    unf_unrel=`${bindir}/thot_repetition_rate -c ${unrelated_tmpfile} -t ${population_tmpfile} | $GREP UNF | $AWK '{printf"%s",$3}'`
fi

# Print results
sample_numw=`$WC ${sample} | $AWK '{printf"%s",$2}'`
echo "* Input Sample"
echo " - Length in words        : ${sample_numw}"
echo " - Unseen n-gram fraction : ${unf_sample}"
echo ""

popsample_numw=`$WC ${popsample_tmpfile} | $AWK '{printf"%s",$2}'`
echo "* Representative Sample"
echo " - Length in words        : ${popsample_numw}"
echo " - Unseen n-gram fraction : ${unf_popsample}"
echo ""

if [ ${u_given} -eq 1 ]; then
    unrel_numw=`$WC ${unrelated_tmpfile} | $AWK '{printf"%s",$2}'`
    echo "* Unrelated Text"
    echo " - Length in words        : ${unrel_numw}"
    echo " - Unseen n-gram fraction : ${unf_unrel}"
    echo ""
fi
