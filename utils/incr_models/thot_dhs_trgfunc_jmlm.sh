# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Target function example to be used with the downhill package.  The
# target function adjusted by this script is the perplexity of 
# language model with Jelinek-Mercer smoothing.

if [ $# -lt 2 ]; then
    echo "Usage: thot_dhs_trgfunc_jmlm <sdir> <w1> ... <wn>"
else
    # Initialize variables
    if [ "${LM}" = "" ]; then LM=${BASEDIR}/LM/e.lm ; fi
    if [ "${TEST}" = "" ]; then TEST=${BASEDIR}/DATA/Es-dev ; fi
    if [ "${ORDER}" = "" ]; then ORDER=3 ; fi
    if [ "${NUMBUCK}" = "" ]; then NUMBUCK=1 ; fi
    if [ "${BUCKSIZE}" = "" ]; then BUCKSIZE=1 ; fi
    if [ "${PPL_WITH_OOVW}" = "" ]; then PPL_WITH_OOVW=1; fi
    if [ "${USE_CACHE_LM}" = "1" ]; then 
        CPERP_FLAG="-cjm"; 
    else
        CPERP_FLAG="-jm"
    fi
    if [ "${QS}" != "" ]; then qs_opt="-qs"; fi

    NON_NEG_CONST=1
    GE1_CONST=1

    # Read parameters
    SDIR=$1
    shift
    weights=""
    while [ $# -gt 0 ]; do
        # Check non-negativity constraints if required
        if [ ${NON_NEG_CONST} -eq 1 ]; then
            neg=`echo "$1" | ${AWK} '{if($1>=0) printf"0\n"; else printf"1\n"}'` 
            if [ $neg -eq 1 ]; then
                echo "(non-negativity constraints violated)" >> ${SDIR}/lm_trgf.ppl
                echo "999999"
                exit 0
            fi
        fi

        # Check ge1 constraints if required
        if [ ${GE1_CONST} -eq 1 ]; then
            ge1=`echo "$1" | ${AWK} '{if($1<1.0) printf"0\n"; else printf"1\n"}'` 
            if [ $ge1 -eq 1 ]; then
                echo "(ge1 constraints violated)" >> ${SDIR}/lm_trgf.ppl
                echo "999999"
                exit 0
            fi
        fi

        # Build weight vector
        weights="${weights} $1"
        
        shift
    done

    # Check files
    if [ ! -f ${LM} ]; then
        echo "Error: LM file ${LM} does not exist" >&2
        exit 1
    fi

    if [ ! -f ${TEST} ]; then
        echo "Error: test file ${TEST} does not exist" >&2
        exit 1
    fi

    # Calculate perplexity
    echo "$ORDER $NUMBUCK $BUCKSIZE $weights" > ${LM}.weights
    $bindir/thot_pbs_ilm_perp -lm ${LM} -c ${TEST} -n ${ORDER} ${CPERP_FLAG} -v1 \
        -o ${SDIR}/verbose_ppl ${qs_opt} "${QS}" -tdir ${SDIR} -sdir ${SDIR}  
    ${bindir}/thot_obtain_info_from_verbose_ppl ${SDIR}/verbose_ppl > ${SDIR}/ppl_info

    ppl_with_oovw=`cat ${SDIR}/ppl_info | grep "Perplexity:" | $AWK '{printf"%s",$2}'`
    ppl_without_oovw=`cat ${SDIR}/ppl_info | grep "Perplexity without OOV" | $AWK '{printf"%s",$NF}'`

    # Calculate target function
    if [ ${PPL_WITH_OOVW} -eq 1 ]; then
        trg_func=${ppl_with_oovw}
    else
        trg_func=${ppl_without_oovw}
    fi

    # Print target function image
    echo ${trg_func}
    echo ${trg_func} >> ${SDIR}/lm_trgf.ppl
fi
