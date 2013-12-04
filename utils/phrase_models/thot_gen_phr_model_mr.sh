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
    echo "Usage: thot_gen_phr_model_mr {-g <GizaAligFileName> [-pml] [-mon]"
    echo "            [-pid | -pc | -ph1] [-lex <gizaFilesPref>]"
    echo "            |-l <phraseModelFilesPrefix> -best-al <bestAligFileName>}" 
    echo "            [-m <maxTrgPhraseLength> [-nospur] [-ms]]"
    echo "            [-c <cutoffValue]"
    if [ "${disable_iterators}" = "yes" ]; then
        echo "            [-mc <maxNoCombAllocInSegmTable>]"
    fi
    echo "            {-o <outputFilesPrefix>} [-v | -v1 | -va]"
    echo "            [-T <tmpdir>] [-la <label>] [--help] [--version]"
    echo ""
    echo "-g <GizaAligFileName>           Name of the alignment file in GIZA format for"
    echo "                                generating a phrase model."
    echo ""
    echo "-pml                            Obtain pseudo-ml model (RF by default)."
    echo ""
    echo "-mon                            Obtain monotone model (non-monotone by default)."
    echo ""
    echo "-pid                            Print inverse and direct models in the same"
    echo "                                table."
    echo ""
    echo "-ph1                            Print output in pharaoh v1.0 format (moses by"
    echo "                                default)."
    echo ""
    echo "-pc                             Print output in Thot native format, which"
    echo "                                includes source and joint counts."
    echo ""
    echo "-lex <gizaFilesPref>            Add lexical components to the translation table"
    echo "                                by means of the IBM models. This option cannot"
    echo "                                be combined with the -pc and the -ph1 options."
    echo ""
    echo "-l <phraseModelFilesPrefix>     Phrase-model file prefix for load. The expected"
    echo "                                format of the files is the one obtained"
    echo "                                with -pc option."
    echo ""
    echo "-best-al <bestAligFileName>     GIZA++ .A3 file for obtaining the best"
    echo "                                alignments using the phrase model."
    echo ""	
    echo "-m <maxTrgPhraseLength>         Set maximum target phrase length (target is the"
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
    echo "-c <cutoffValue>                Remove from the translation table those pairs"
    echo "                                whose count for the source phrase is lower or"
    echo "                                equal to cutoffValue (source is the source"
    echo "                                language of the GIZA alignment file)."
    echo ""
    if [ "${disable_iterators}" = "yes" ]; then
        echo "-mc <maxCombsInSegmTable>       During a pseudo-ml estimation process,"
        echo "                                this parameter sets the maximum number of "
        echo "                                segmentations (in millions) which can be alloc-"
        echo "                                ated in the segmentation table (20 by default)."
        echo ""
    fi
    echo "-o <outputFilesPrefix>          Set output files prefix name."
    echo ""
    echo "-v | -v1 | -va                  Verbose mode | more verbosity | verbose mode in"
    echo "                                Aachen alignment format"	
    echo ""
    echo "-T <tmpdir>                     Use <tmpdir> for temporaries instead of /tmp"
    echo ""
    echo "-la <label>                     Add the label <label> to each entry of a ttable"
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
    # If sh is being used as command interpreter, PIPESTATUS variable is
    # not available
    command_interp=$(basename "${BASH}")
    if [ ${command_interp} = "sh" ]; then
        return 0
    else
        # test if there is at least one command to exit with a non-zero status
        for pipe_status_elem in ${PIPESTATUS[*]}; do 
            if test ${pipe_status_elem} -ne 0; then 
                return 1; 
            fi 
        done
        return 0
    fi
}

lex_func()
{
    if [ ${lex_given} -eq 0 ]; then
        awk '{printf"%s\n",$0}'
    else
        ${bindir}/thot_score_phr_ibm ${lex_pref}
    fi
}

thot_pars=""
g_given=0
o_given=0
m_correct=1
estimation="RF"
out_format=""
lex_given=0
l_given=0
bestal_given=0
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
        "-pml") thot_pars="$thot_pars -pml"
            estimation="PML"
            ;;
        "-g") shift
            if [ $# -ne 0 ]; then
                a3_file=$1
                g_given=1
            fi
            ;;
        "-l") shift
            if [ $# -ne 0 ]; then
                modelpref=$1
                l_given=1
            fi
            ;;
        "-best-al") shift
            if [ $# -ne 0 ]; then
                ba3_file=$1
                bestal_given=1
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
        "-mon") thot_pars="$thot_pars -mon"
            ;;
        "-pml") thot_pars="$thot_pars -pml"
            estimation="PML"
            ;;
        "-nospur") thot_pars="$thot_pars -nospur"
            ;;
        "-ms") thot_pars="$thot_pars -ms"
            ;;
        "-debug") debug="-debug"
            ;;
        "-v") thot_pars="$thot_pars -v"
            ;;
        "-v1") thot_pars="$thot_pars -v1"
            ;;
        "-va") thot_pars="$thot_pars -va"
            ;;
        "-ph1") out_format="-ph1"
            ;;
        "-pc") out_format="-pc"
            ;;
        "-pid") out_format="-pid"
            ;;
        "-lex") shift
            lex_given=1
            if [ $# -ne 0 ]; then
                lex_pref=$1
            else
                lex_pref=""
            fi
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
if [ ${g_given} -eq 0 -o ${l_given} -eq 0 ]; then
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

if [ ${lex_given} -eq 1 -a "${out_format}" = "-pc" ]; then
    echo "Error: -lex and -pc options cannot be combined" >&2
    exit 1
fi

if [ ${lex_given} -eq 1 -a "${out_format}" = "-ph1" ]; then
    echo "Error: -lex and -ph1 options cannot be combined" >&2
    exit 1
fi

if [ ${lex_given} -eq 1 ]; then
    if [ "${lex_pref}" = "" ]; then
        echo "Error: prefix for -lex option not given" >&2
        exit 1
    else
        for suff in ".trn.src.vcb" ".trn.trg.vcb" ".ti.final" ".t3.final"; do
            if [ ! -f "${lex_pref}${suff}" ]; then
                echo "Error: file ${lex_pref}${suff} required by the -lex option does not exist" >&2
                exit 1
            fi
        done
    fi
fi

# parameters are ok

if [ ${l_given} -eq 1 ]; then
    if [ ${bestal_given} -eq 1 ]; then
            # load phrase model and obtain the best alignments
        ${bindir}/thot_gen_phr_model -l ${modelpref} -best-al ${ba3_file} ${thot_pars} -o ${output} || exit 1
    else
        echo "Error: -best-al option not given!" >&2
        exit 1
    fi
else
        # train phrase model
    echo "Training model..." 1>&2

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

    echo "*** Process started at: " `date` > $TMP/log
    echo "Spliting input: ${a3_file}..." >> $TMP/log

    # Split input into chunks and process them separately
    ${SPLIT} -l ${chunk_size} ${a3_file} $TMP/chunk\_ || exit 1
    c=1
    for i in `ls $TMP/chunk\_*`; do
        chunk=`${BASENAME} $i`
        ${bindir}/thot_gen_phr_model -g $TMP/${chunk} ${thot_pars} -o $TMP/${chunk} -pc || exit 1
        ${AWK} -v cn=$c '{printf"%s %s\n",$0,cn}' $TMP/${chunk}.ttable >> $TMP/counts
        if [ "${estimation}" = "PML" ]; then
            cat $TMP/${chunk}.seglentable >> $TMP/seglentable
        fi
        cat $TMP/${chunk}.log>> $TMP/log
        
        rm $TMP/${chunk} $TMP/${chunk}.log $TMP/${chunk}.ttable
        c=`expr $c + 1`
    done

    # Merge counts and print the models
    echo "Merging counts..." 1>&2
    echo "Merging process started at: " `date` >> $TMP/log

    export LC_ALL=""
    export LC_COLLATE=C
    export LC_NUMERIC=C

    if [ "${out_format}" = "-pc" ]; then
            # output format = -pc
         if [ ${label_given} -eq 0 ]; then
              sort ${SORT_TMP} -t " " ${sortpars} $TMP/counts | ${bindir}/thot_merge_counts \
                  | ${bindir}/thot_cut_ttable -c $cutoff > ${output}.ttable ; pipe_fail || exit 1
         else
             sort ${SORT_TMP} -t " " ${sortpars} $TMP/counts | ${bindir}/thot_merge_counts | ${bindir}/thot_cut_ttable -c $cutoff \
                 | ${AWK} -v label=$label '{printf"%s %s\n",$0,label}' > ${output}.ttable ; pipe_fail || exit 1
         fi
    else
        if [ "${out_format}" = "-ph1" ]; then
                # output format = pharaoh v1.0
            sort ${SORT_TMP} -t " " ${sortpars} $TMP/counts | ${bindir}/thot_merge_counts \
                | ${bindir}/thot_cut_ttable -c $cutoff > $TMP/pc.ttable ; pipe_fail || exit 1
            ${bindir}/thot_counts_to_prob $TMP/pc.ttable> ${output}.ttable || exit 1
            ${bindir}/thot_invert_ttable -t $TMP/pc.ttable ${SORT_TMP} > ${output}.ittable ; pipe_fail || exit 1
        else
            if [ "${out_format}" = "-pid" ]; then
                # moses inverse-direct model output format
                sort ${SORT_TMP} -t " " ${sortpars} $TMP/counts | ${bindir}/thot_merge_counts | ${bindir}/thot_cut_ttable -c $cutoff \
                    | ${bindir}/thot2pidttable ${SORT_TMP} | lex_func > ${output}.ttable ; pipe_fail || exit 1
                
            else
                # moses output format
                sort ${SORT_TMP} -t " " ${sortpars} $TMP/counts | ${bindir}/thot_merge_counts | ${bindir}/thot_cut_ttable -c $cutoff \
                    | ${bindir}/thot_flip_phr | ${bindir}/thot_counts_to_prob | lex_func > ${output}.ttable ; pipe_fail || exit 1
            fi
        fi
    fi
        
    if [ "${estimation}" = "PML" ]; then
        ${bindir}/thot_merge_seglen_counts $TMP/seglentable > ${output}.seglentable || exit 1
    fi
    
    echo "*** Process finished at: " `date` >> $TMP/log
    mv $TMP/log ${output}.log
fi
