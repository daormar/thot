# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Allows to train very large corpus using thot_gen_phr_model

print_desc()
{
    echo "thot_gen_phr_model_mr written by Daniel Ortiz"
    echo "thot_gen_phr_model_mr implements phrase model estimation using map reduce"
    echo "type \"thot_gen_phr_model_mr --help\" to get usage information."
}

version()
{
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

usage()
{
    echo "Usage: thot_gen_phr_model_mr {-g <string> [-brf]"
    echo "            [-m <int>]"
    echo "            [-c <float>]"
    echo "            {-o <string>} [-v | -v1 ]"
    echo "            [-T <string>] [-la <string>] [--help] [--version]"
    echo ""
    echo "-g <string>                     Name of the alignment file in GIZA format for"
    echo "                                generating a phrase model."
    echo ""
    echo "-brf                            Obtain bisegmentation-based RF model (RF by"
    echo "                                default)."
    echo ""
    echo "-m <int>                        Set maximum target phrase length (target is the"
    echo "                                target language of the GIZA alignment file)."
    echo ""
    echo "-nospur                         Do not take into account the spurious words when"
    echo "                                calculating the source phrase length (source is"
    echo "                                the source language of the GIZA alignment file)."
    echo ""
    echo "-ms                             Also impose the maximum phrase-length"
    echo "                                constraint to the source phrase (source is the"
    echo "                                source language of the GIZA alignment file)."
    echo ""
    echo "-c <float>                      Remove from the translation table those pairs"
    echo "                                whose count for the source phrase is lower or"
    echo "                                equal to cutoffValue (source is the source"
    echo "                                language of the GIZA alignment file)."
    echo ""
    echo "-o <string>                     Set output files prefix name."
    echo ""
    echo "-v | -v1                        Verbose mode | more verbosity"	
    echo ""
    echo "-T <string>                     Use <tmpdir> for temporaries instead of /tmp"
    echo ""
    echo "-la <string>                    Add the label <label> to each entry of a ttable"
    echo "                                when using the -pc option. This option is used"
    echo "                                internally by pbs_thot."
    echo ""
    echo "-debug                          After ending, do not delete temporary files"
    echo "                                (for debugging purposes)"
    echo ""
    echo "--help                          Display this help and exit"
    echo ""
    echo "--version                       Output version information and exit"
    echo ""
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

thot_pars=""
g_given=0
o_given=0
m_correct=1
estimation="RF"
out_format="-pc"
tmpdir="/tmp"
label_given=0
debug=""
cutoff=0

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
        "-brf") thot_pars="$thot_pars -brf"
            estimation="BRF"
            ;;
        "-g") shift
            if [ $# -ne 0 ]; then
                a3_file=$1
                g_given=1
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                output=$1
                o_given=1
            fi
            ;;
        "-m") thot_pars="$thot_pars -m"
            shift
            if [ $# -ne 0 ]; then
                thot_pars="$thot_pars $1"
                m_correct=1
            fi
            ;;
        "-c") shift
            if [ $# -ne 0 ]; then
                cutoff=$1
            fi
            ;;
        "-debug") debug="-debug"
            ;;
        "-v") thot_pars="$thot_pars -v"
            ;;
        "-v1") thot_pars="$thot_pars -v1"
            ;;
        "-T") shift
            if [ $# -ne 0 ]; then
                tmpdir=$1
            else
                tmpdir="/tmp"
            fi
            ;;
        "-la") shift
            if [ $# -ne 0 ]; then
                label_given=1
                label=$1
            fi
            ;;
    esac
    shift
done

# verify parameters
if [ ${g_given} -eq 1 ]; then
    # verify that -g file exist
    if [ ${g_given} -eq 1 -a  ! -f  "${a3_file}" ];then
        echo "Error: file "$a3_file" does not exist " >&2
        exit 1
    fi
else
    usage
    exit 1
fi

if [ ${o_given} -eq 0 -o ${m_correct} -eq 0 ]; then
    # invalid parameters 
    usage
    exit 1
fi

# parameters are ok

# train phrase model
# echo "Training model..." >&2

# set chunk size
chunk_size=90000

# Set TMP directory
TMP="${tmpdir}/thot_gen_phr_model_mr_tmp_${PPID}_$$"
if [ "$debug" != "-debug" ]; then
    trap "rm -rf $TMP 2>/dev/null" EXIT
fi
mkdir $TMP || { echo "Error: temporary directory cannot be created" >&2 ; exit 1; }
# Set tmp dir of the sort command if possible
if test ${sortT} = "yes"; then
    SORT_TMP="-T $TMP"
else
    SORT_TMP=""
fi

echo "+++ Process started at: " `date` > $TMP/log
echo "Spliting input: ${a3_file}..." >> $TMP/log
echo "Spliting input: ${a3_file}..." >&2

# Split input into chunks and process them separately
${SPLIT} -a 4 -l ${chunk_size} ${a3_file} $TMP/chunk\_ || exit 1
c=1
for i in `ls $TMP/chunk\_*`; do
    chunk=`${BASENAME} $i`

    echo "Processing chunk ${chunk}" >> $TMP/log
    echo "Processing chunk ${chunk}" >&2

    ${bindir}/thot_gen_phr_model -g $TMP/${chunk} ${thot_pars} -o $TMP/${chunk} || exit 1
    ${AWK} -v cn=$c '{printf"%s %s\n",$0,cn}' $TMP/${chunk}.ttable >> $TMP/counts || exit 1
    if [ "${estimation}" = "BRF" ]; then
        cat $TMP/${chunk}.seglentable >> $TMP/seglentable || exit 1
    fi
    
    rm $TMP/${chunk} $TMP/${chunk}.ttable || exit 1
    c=`expr $c + 1`
done

# Merge counts and print the models
echo "Merging counts..." >> $TMP/log
echo "Merging counts..." >&2

# output format = -pc
if [ ${label_given} -eq 0 ]; then
    LC_ALL=C ${SORT} ${SORT_TMP} -t " " ${sortpars} $TMP/counts | ${bindir}/thot_merge_counts \
        | ${bindir}/thot_cut_ttable -c $cutoff > ${output}.ttable ; pipe_fail || exit 1
else
    LC_ALL=C ${SORT} ${SORT_TMP} -t " " ${sortpars} $TMP/counts | ${bindir}/thot_merge_counts | ${bindir}/thot_cut_ttable -c $cutoff \
        | ${AWK} -v label=$label '{printf"%s %s\n",$0,label}' > ${output}.ttable ; pipe_fail || exit 1
fi

if [ "${estimation}" = "BRF" ]; then
    ${bindir}/thot_merge_seglen_counts $TMP/seglentable > ${output}.seglentable || exit 1
fi

echo "+++ Process finished at: " `date` >> $TMP/log
mv $TMP/log ${output}.log
