# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Executes language and translation model tuning.

# \textbf{Categ}: modelling

########
print_desc()
{
    echo "thot_rnnlm_rescore written by Daniel Ortiz"
    echo "thot_rnnlm_rescore rescore n-best list using an rnn-lm"
    echo "type \"thot_rnnlm_rescore --help\" to get usage information"
}

########
version()
{
    echo "thot_rnnlm_rescore is part of the exper package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_rnnlm_rescore      -t <string> -n <string> -r <string>"
    echo "                        -o <string> [-voc <string>] [-removelm]"
    echo "                        [-tdir <string>] [-debug] [--help] [--version]"
    echo ""
    echo "-t <string>             File with target sentences."
    echo "-n <string>             Directory containing n-best lists."
    echo "-r <string>             RNN LM file."
    echo "-o <string>             Output directory common to all processors."
    echo "-voc <string>           File with training vocabulary. Words occuring in"
    echo "                        the n-best lists outside vocabulary are replaced by"
    echo "                        <unk>."
    echo "--removelm              Remove previous language model component (lm) from"
    echo "                        n-best lists."
    echo "-tdir <string>          Directory for temporary files (/tmp by default)."
    echo "                        NOTES:"
    echo "                         a) give absolute paths when using pbs clusters"
    echo "                         b) ensure there is enough disk space in the partition"
    echo "-debug                  After ending, do not delete temporary files"
    echo "                        (for debugging purposes)"
    echo "--help                  Display this help and exit."
    echo "--version               Output version information and exit."
}

########
get_sentid()
{
    local_file=$1
    echo ${local_file} | $AWK -F "." '{printf"%s",$1}' | $AWK -F "_" '{printf"%s",$2}'
}

########
nblist_voc_filter()
{
    # Handle -voc option
    if [ ${voc_given} -eq 0 ]; then
        cat
    else
        ${bindir}/thot_replace_oov_with_unk -v ${vocab}
    fi    
}

########
nblist_removelm_filter()
{
    # Handle -removelm option
    if [ ${removelm_given} -eq 0 ]; then
        cat
    else
        ${bindir}/thot_remove_nbl_component -c lm
    fi    
}

########
process_nblist()
{
    local_nblist=$1
    cat ${local_nblist} | nblist_voc_filter | nblist_removelm_filter
}

########
compute_rnnlm_scores_for_nblists()
{
    # Process n-best lists
    for nblfile in ${nblistdir}/*.nbl; do
        # Extract translations from n-best list
        nblfile_base=`${BASENAME} $nblfile`
        ${bindir}/thot_extract_sents_from_nbl < $nblfile > ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.trans_aux

        # Process n-best list
        process_nblist ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.trans_aux > ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.trans

        # Remove temporary files
        rm ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.trans_aux

        # Score translations
        ${FASTER_RNNLM_BUILD_DIR}/rnnlm -rnnlm ${rnnlm} \
            -test ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.trans \
            > ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.rnnlm_scores 2> ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.log
    done
}

########
fast_compute_rnnlm_scores_for_nblists()
{
    # Process n-best lists
    for nblfile in ${nblistdir}/*.nbl; do
        # Extract translations from n-best list
        nblfile_base=`${BASENAME} $nblfile`
        ${bindir}/thot_extract_sents_from_nbl < $nblfile > ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.trans_aux

        # Process n-best list
        process_nblist ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.trans_aux > ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.trans

        # Remove temporary files
        rm ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.trans_aux

        # Add translations to all translations file
        cat ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.trans >> ${TDIR_RNNLM_RESCORE}/rnnlm_scores/all_trans
    done

    # Score all translations file
    ${FASTER_RNNLM_BUILD_DIR}/rnnlm -rnnlm ${rnnlm} \
                             -test ${TDIR_RNNLM_RESCORE}/rnnlm_scores/all_trans \
                             > ${TDIR_RNNLM_RESCORE}/rnnlm_scores/all_trans.rnnlm_scores 2> ${TDIR_RNNLM_RESCORE}/rnnlm_scores/all_trans.log

    # Create score files for each n-best list
    offset=0
    scrfile=${TDIR_RNNLM_RESCORE}/rnnlm_scores/all_trans.rnnlm_scores
    for nblfile in ${nblistdir}/*.nbl; do
        nblfile_base=`${BASENAME} $nblfile`
        numtrans=`$WC -l ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.trans | $AWK '{printf"%s",$1}'`
        offset=`expr $offset + $numtrans`
        $HEAD -${offset} $scrfile | $TAIL -${numtrans} > ${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.rnnlm_scores
    done
}

########
create_aug_nblists()
{
    # Process n-best lists
    for nblfile in ${nblistdir}/*.nbl; do
        nblfile_base=`${BASENAME} $nblfile`
        file_with_new_score=${TDIR_RNNLM_RESCORE}/rnnlm_scores/${nblfile_base}.rnnlm_scores
        ${bindir}/thot_augment_nblist -n $nblfile -a rnnlm -s ${file_with_new_score} > ${TDIR_RNNLM_RESCORE}/augmented_nblists/${nblfile_base}
    done
}

########
obtain_loglin_nonneg_const()
{
    _smtw_names=`obtain_smtweights_names`
    echo "${_smtw_names}" | $AWK '{for(i=1;i<=NF;++i) if($i=="wpw" || $i=="tseglenw") printf"0 "; else printf"1 "}'
}

##################
obtain_smtweights_names()
{
    # Process n-best lists
    for nblfile in ${TDIR_RNNLM_RESCORE}/augmented_nblists/*.nbl; do
        head -1 $nblfile | $AWK -F " , " '{for(i=1;i<=NF;++i){nf=split($i,winfo," "); printf"%s ",winfo[nf-1]}}'
        break
    done
}

##################
obtain_smtweights_values()
{
    # Process n-best lists
    for nblfile in ${TDIR_RNNLM_RESCORE}/augmented_nblists/*.nbl; do
        head -1 $nblfile | $AWK -F " , " '{for(i=1;i<=NF;++i){nf=split($i,winfo," "); printf"%s ",winfo[nf]}}'
        break
    done
}

########
obtain_loglin_dhs_va_opt_values()
{
    _smtw_names=`obtain_smtweights_names`
#    echo "${_smtw_names}" | $AWK '{for(i=1;i<=NF;++i) if($i=="swlenliw") printf"0 "; else printf"-0 "}'
    echo "${_smtw_names}" | $AWK '{for(i=1;i<=NF;++i) printf"-0 "}'
}

########
obtain_loglin_iv_opt_values()
{
    _smtw_names=`obtain_smtweights_names`
    echo "${_smtw_names}" | $AWK '{for(i=1;i<=NF;++i) printf"1 "}'
}

########
loglin_downhill()
{
    # Export required variables
    export NBL_DIR=${TDIR_RNNLM_RESCORE}/augmented_nblists
    export REF=$tcorpus

    # Generate information for weight initialisation
    export NON_NEG_CONST="`obtain_loglin_nonneg_const`"
    va_opt="`obtain_loglin_dhs_va_opt_values`"
    iv_opt="`obtain_loglin_iv_opt_values`"

    # Execute tuning algorithm
    tdir=/tmp
    ${bindir}/thot_dhs_min -tdir $tdir -va ${va_opt} -iv ${iv_opt} \
        -ftol ${ftol_loglin} -o ${outd}/llweights_tune -u ${bindir}/thot_dhs_nbl_rescore ${debug_opt} || return 1
}

########
obtain_loglin_upd_va_opt_values()
{
    _smtw_names=`obtain_smtweights_names`
    echo "${_smtw_names}" | $AWK '{for(i=1;i<=NF;++i) printf"1 "}'
}

########
loglin_upd()
{
    # Obtain file with n-best lists file names
    for nblfile in ${TDIR_RNNLM_RESCORE}/augmented_nblists/*.nbl; do
        echo $nblfile >> ${TDIR_RNNLM_RESCORE}/augmented_nblists/nbl_filenames.txt
    done

    # Obtain weight related information
    llweights="`obtain_smtweights_values`"
    va_opt="`obtain_loglin_upd_va_opt_values`"
    
    # Update weights given n-best lists
    $bindir/thot_ll_weight_upd_nblist -w ${llweights} -va ${va_opt} \
                                      -nb ${TDIR_RNNLM_RESCORE}/augmented_nblists/nbl_filenames.txt \
                                      -r ${tcorpus} > ${outd}/llweights_tune.out 2>${outd}/llweights_tune.log || return 1

    # Obtain score with optimized weights
    export NBL_DIR=${TDIR_RNNLM_RESCORE}/augmented_nblists
    export REF=$tcorpus
    upd_weights="`cat ${outd}/llweights_tune.out | $AWK -F ": " '{print $2}'`"
    ${bindir}/thot_dhs_nbl_rescore ${TDIR_RNNLM_RESCORE} ${upd_weights} > ${outd}/llweights_tune.scr
}

##################

if [ $# -eq 0 ]; then
    print_desc
    exit 1
fi

# Read parameters
t_given=0
n_given=0
r_given=0
o_given=0
voc_given=0
removelm_given=0
tdir_given=0
tdir="/tmp"
debug=0

while [ $# -ne 0 ]; do
    case $1 in
        "--help") usage
            exit 0
            ;;
        "--version") version
            exit 0
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                tcorpus=$1
                t_given=1
            fi
            ;;
        "-n") shift
            if [ $# -ne 0 ]; then
                nblistdir=$1
                n_given=1
            fi
            ;;
        "-r") shift
            if [ $# -ne 0 ]; then
                rnnlm=$1
                r_given=1
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                outd=$1
                o_given=1
            fi
            ;;
        "-voc") shift
            if [ $# -ne 0 ]; then
                vocab=$1
                voc_given=1
            fi
            ;;
        "-removelm") removelm=1
            ;;
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1
                tdir_given=1
            fi
            ;;
        "-debug") debug=1
            debug_opt="-debug"
            ;;
    esac
    shift
done

# Check parameters
if [ ${t_given} -eq 0 ]; then        
    echo "Error! -t parameter not given!" >&2
    exit 1
else
    if [ ! -f ${tcorpus} ]; then
        echo "Error! file ${tcorpus} does not exist" >&2
        exit 1
    fi
fi

if [ ${n_given} -eq 0 ]; then
    echo "Error! -n parameter not given!" >&2
    exit 1
fi

if [ ${r_given} -eq 0 ]; then
    echo "Error! -r parameter not given!" >&2
    exit 1
fi

if [ ${o_given} -eq 0 ]; then
    echo "Error! -o parameter not given!" >&2
    exit 1
else
    if [ -d ${outd} ]; then
        echo "Warning! output directory does exist" >&2 
        # echo "Error! output directory should not exist" >&2 
        # exit 1
    else
        mkdir -p ${outd} || { echo "Error! cannot create output directory" >&2; return 1; }
    fi
fi

if [ ${voc_given} -eq 1 ]; then        
    if [ ! -f ${vocab} ]; then
        echo "Error! file ${vocab} does not exist" >&2
        exit 1
    fi
fi

if [ ${tdir_given} -eq 1 ]; then
    if [ ! -d ${tdir} ]; then
        echo "Error! directory ${tdir} does not exist" >&2
        exit 1            
    fi
fi

# Set default parameters
ftol_lm=0.1
ftol_loglin=0.001

# Create tmp directory
if [ ! -d ${tdir} ]; then
    echo "Error: tmp directory does not exist" >&2
    return 1;
fi

TDIR_RNNLM_RESCORE=`${MKTEMP} -d ${tdir}/thot_rnnlm_rescore_XXXXXX`

# Remove temp directories on exit
if [ "$debug_opt" != "-debug" ]; then
    trap "rm -rf $TDIR_RNNLM_RESCORE 2>/dev/null" EXIT
fi

# Create additional directories
mkdir ${TDIR_RNNLM_RESCORE}/rnnlm_scores
mkdir ${TDIR_RNNLM_RESCORE}/augmented_nblists

# Compute rnnlm scores
echo "* Computing rnn-lm scores for n-best lists" >&2
fast_compute_rnnlm_scores_for_nblists || exit 1
echo "" >&2

# Create augmented n-best lists
echo "* Creating augmented n-best lists" >&2
create_aug_nblists || exit 1
echo "" >&2

# Update log-linear weights
echo "* Updating log-linear weights for augmented n-best lists" >&2
#loglin_downhill || exit 1
loglin_upd || exit 1
echo "" >&2
