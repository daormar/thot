# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Executes n-best list weight tuning.

# \textbf{Categ}: modelling

########
print_desc()
{
    echo "thot_nblist_tune written by Daniel Ortiz"
    echo "thot_nblist_tune tune weights for a set of n-best lists"
    echo "type \"thot_nblist_tune --help\" to get usage information"
}

########
version()
{
    echo "thot_nblist_tune is part of the exper package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_nblist_tune        -t <string> -n <string> -o <string> [-dhs]"
    echo "                        [-tdir <string>] [-debug] [--help] [--version]"
    echo ""
    echo "-t <string>             File with target sentences."
    echo "-n <string>             Directory containing n-best lists."
    echo "-o <string>             Output directory common to all processors."
    echo "-dhs                    Use downhill simplex to carry out the tuning."
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
obtain_loglin_nonneg_const()
{
    _smtw_names=`obtain_smtweights_names`
    echo "${_smtw_names}" | $AWK '{for(i=1;i<=NF;++i) if($i=="wpw" || $i=="tseglenw") printf"0 "; else printf"1 "}'
}

##################
obtain_smtweights_names()
{
    # Process n-best lists
    for nblfile in ${nblistdir}/*.nbl; do
        head -1 $nblfile | $AWK -F " , " '{for(i=1;i<=NF;++i){nf=split($i,winfo," "); printf"%s ",winfo[nf-1]}}'
        break
    done
}

##################
obtain_smtweights_values()
{
    # Process n-best lists
    for nblfile in ${nblistdir}/*.nbl; do
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
    export NBL_DIR=${nblistdir}
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
    # Remove previously existing files
    if [ -f ${outd}/nbl_filenames.txt ]; then
        rm ${outd}/nbl_filenames.txt
    fi
    
    # Obtain file with n-best lists file names
    for nblfile in ${nblistdir}/*.nbl; do
        echo $nblfile >> ${outd}/nbl_filenames.txt
    done

    # Obtain weight related information
    llweights="`obtain_smtweights_values`"
    va_opt="`obtain_loglin_upd_va_opt_values`"
    
    # Update weights given n-best lists
    $bindir/thot_ll_weight_upd_nblist -w ${llweights} -va ${va_opt} \
                                      -nb ${outd}/nbl_filenames.txt \
                                      -r ${tcorpus} > ${outd}/llweights_tune.out 2>${outd}/llweights_tune.log || return 1

    # Obtain score with optimized weights
    export NBL_DIR=${nblistdir}
    export REF=$tcorpus
    upd_weights="`cat ${outd}/llweights_tune.out | $AWK -F ": " '{print $2}'`"
    ${bindir}/thot_dhs_nbl_rescore ${TDIR_NBLIST_TUNE} ${upd_weights} > ${outd}/llweights_tune.scr
}

##################

if [ $# -eq 0 ]; then
    print_desc
    exit 1
fi

# Read parameters
n_given=0
o_given=0
voc_given=0
dhs_given=0
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
        "-o") shift
            if [ $# -ne 0 ]; then
                outd=$1
                o_given=1
            fi
            ;;
        "-dhs") dhs_given=1
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

TDIR_NBLIST_TUNE=`${MKTEMP} -d ${tdir}/thot_nblist_tune_XXXXXX`

# Remove temp directories on exit
if [ "$debug_opt" != "-debug" ]; then
    trap "rm -rf $TDIR_NBLIST_TUNE 2>/dev/null" EXIT
fi

# Update log-linear weights
echo "* Updating log-linear weights for n-best lists" >&2
if [ ${dhs_given} -eq 1 ]; then
    loglin_downhill || exit 1
else
    loglin_upd || exit 1
fi
echo "" >&2
