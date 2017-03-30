# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Target function example to be used with the downhill package.  The
# target function adjusted by this script is the translation quality
# measure provided by thot_scorer for a set of n-best lists.

########
calc_nnc_pen()
{
    we="$1"
    nnc="$2"
    pen_fact=$3
    echo "$we" | $AWK -v nnc="${nnc}" -v pen_fact=${pen_fact}\
                      'BEGIN{
                             result=0;
                             split(nnc,nnc_arr," ")
                            }
                            {   
                             for(i=1;i<=NF;++i)
                             {
                              if($i<0 && nnc_arr[i]==1) 
                               result+=$i*pen_fact*(-1)
                             }
                            }
                         END{
                             printf"%f",result
                            }'
}

########
gen_trans()
{
    # Remove file with translations
    if [ -f ${SDIR}/smt_trgf.trans ]; then
        rm ${SDIR}/smt_trgf.trans
    fi

    # Extract translations from n-best lists
    for nblist in ${NBL_DIR}/*.nbl; do
        # Obtain best translation from n-best list
        ${bindir}/thot_obtain_best_trans_from_nbl $nblist "$weights" >> ${SDIR}/smt_trgf.trans
    done
}

########
evaluate()
{
    ${bindir}/thot_scorer -r ${REF} -t  ${SDIR}/smt_trgf.trans >> ${SDIR}/smt_trgf.score
    SCORE=`tail -1 ${SDIR}/smt_trgf.score | ${AWK} '{printf"%f\n",1-$2}'`
    # Print target function value
    echo "${SCORE} ${nnc_pen}" | $AWK '{printf"%f\n",$1+$2}'
}

########

if [ $# -lt 2 ]; then
    echo "Usage: thot_dhs_nbl_rescore <sdir> <w1> ... <wn>"
else
    # Initialize variables
    if [ "${NBL_DIR}" = "" ]; then NBL_DIR=nbl/; fi
    if [ "${REF}" = "" ]; then REF="server.cfg" ; fi
    if [ "${NNC_PEN_FACTOR}" = "" ]; then NNC_PEN_FACTOR=1000; fi

    # Check variables
    if [ ! -d ${NBL_DIR} ]; then
        echo "ERROR: directory ${NBL_DIR} does not exist" >&2
        exit 1
    fi

    if [ ! -f ${REF} ]; then
        echo "ERROR: file ${REF} does not exist" >&2
        exit 1
    fi

    # Read parameters
    SDIR=$1
    shift
    NUMW=$#
    weights=""
    while [ $# -gt 0 ]; do
        # Build weight vector
        weights="${weights} $1"
        shift
    done

    # Obtain non-negativity constraints penalty (non-negativity
    # constraints can be activated for each individual weight by means
    # of the environment variable NON_NEG_CONST, which contains a bit
    # vector; a value of 1 for i'th vector means that the i'th weight
    # must be positive)
    nnc_pen=0
    if [ ! "${NON_NEG_CONST}" = "" ]; then
        nnc_pen=`calc_nnc_pen "${weights}" "${NON_NEG_CONST}" ${NNC_PEN_FACTOR}`
    fi
    
    # Generate translations
    gen_trans

    # Evaluate results
    evaluate
fi
