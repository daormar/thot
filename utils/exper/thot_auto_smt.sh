# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Automatically obtains statistical machine translation results.

########
print_desc()
{
    echo "thot_auto_smt written by Daniel Ortiz"
    echo "thot_auto_smt automatically obtains statistical machine translation results"
    echo "type \"thot_auto_smt --help\" to get usage information"
}

########
version()
{
    echo "thot_auto_smt is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_auto_smt     [-pr <int>]"
    echo "                  -s <string> -t <string> -o <string>"
    echo "                  [--skip-clean] [--tok] [--lower] [--categ] [--no-lim]"
    echo "                  [--no-trans] [-n <int>] [-nit <int>]"
    echo "                  [-cpr <float>] [-m <int>] [-ao <string>]"
    if [ "${KENLM_BUILD_DIR}" != "no" ]; then
        echo "                  [-kenlm]"
    fi
    if [ ! -z "${LEVELDB_LIB}" ]; then
        echo "                  [-ldblm]"
    fi
    if [ ! -z "${LDB_CXX}" ]; then
        echo "                  [-bdbtm]"
    fi
    if [ ! -z "${LEVELDB_LIB}" ]; then
        echo "                  [-ldbtm]"
    fi
    echo "                  [-qs <string>] [-tdir <string>]"
    echo "                  [-sdir <string>] [-debug] [--help] [--version]"
    echo ""
    echo "-pr <int>         Number of processors (1 by default)"
    echo "-s <string>       Prefix of files with source sentences (the"
    echo "                  following suffixes are assumed: .train, .dev and .test)"
    echo "-t <string>       Prefix of files with target sentences (the"
    echo "                  following suffixes are assumed: .train, .dev and .test)"
    echo "-o <string>       Output directory common to all processors"
    echo "--skip-clean      Skip corpus cleaning stage"
    echo "--tok             Execute tokenization stage"
    echo "--lower           Execute lowercasing stage"
    echo "--categ           Execute categorization stage"
    echo "--no-lim          Do not limit size of files used to train recaser and"
    echo "                  detokenizer (requires more memory)"
    echo "--no-trans        Do not generate translations"
    echo "-n <int>          Order of the n-gram language models (4 by default)"
    echo "-nit <int>        Number of iterations of the EM algorithm when training"
    echo "                  single word models (5 by default)"
    echo "-cpr <float>      Pruning parameter used during the estimation of single"
    echo "                  word alignment models (0.00001 by default)"
    echo "-m <int>          Maximum target phrase length during phrase model"
    echo "                  estimation (10 by default)"
    echo "-ao <string>      Operation between alignments to be executed"
    echo "                  (and|or|sum|sym1|sym2|grd)."
    if [ ! "${KENLM_BUILD_DIR}" = "no" ]; then
        echo "-kenlm            Generate on-disk language model in KenLM format."
    fi
    if [ ! -z "${LEVELDB_LIB}" ]; then
        echo "-ldblm            Generate on-disk language model in LevelDB format."
    fi
    if [ ! -z "${LDB_CXX}" ]; then
        echo "-bdbtm            Generate on-disk translation model in BDB format"
    fi    
    if [ ! -z "${LEVELDB_LIB}" ]; then
        echo "-ldbtm            Generate on-disk translation model in LevelDB format."
    fi
    echo "-qs <string>      Specific options to be given to the qsub"
    echo "                  command (example: -qs \"-l pmem=1gb\")"
    echo "                  NOTES:"
    echo "                        a) ignore this if not using a PBS cluster"
    echo "                        b) -qs option may be crucial to ensure the correct"
    echo "                           execution of the tool. The main purpose of -qs"
    echo "                           is to reserve the required cluster resources."
    echo "                           If the necessary resources are not met the"
    echo "                           execution will abort."
    echo "                           Resources are reserved in different ways depending"
    echo "                           on the cluster software. For instance, if using SGE"
    echo "                           software, -qs \"-l h_vmem=1G,h_rt=10:00:00\","
    echo "                           requests 1GB of virtual memory and a time limit"
    echo "                           of 10 hours" 
    echo "-tdir <string>    Directory for temporary files (/tmp by default)."
    echo "                  NOTES:"
    echo "                        a) give absolute paths when using pbs clusters"
    echo "                        b) ensure there is enough disk space in the partition"
    echo "-sdir <string>    Absolute path of a directory common to all"
    echo "                  processors. If not given, \$HOME will be used."
    echo "                  NOTES:"
    echo "                        a) give absolute paths when using pbs clusters"
    echo "                        b) ensure there is enough disk space in the partition"
    echo "-debug            After ending, do not delete temporary files"
    echo "                  (for debugging purposes)"
    echo "--help            Display this help and exit"
    echo "--version         Output version information and exit"
}

########
is_absolute_path()
{
    case $1 in
        /*) echo 1 ;;
        *) echo 0 ;;
    esac
}

########
get_absolute_path()
{
    file=$1
    # Check if an absolute path was given
    absolute=`is_absolute_path $file`
    if [ "$absolute" -eq 1 ]; then
        echo "$file"
    else
        oldpwd=$PWD
        basetmp=`"$BASENAME" "$PWD/$file"`
        dirtmp=`"$DIRNAME" "$PWD/$file"`
        cd "$dirtmp"
        result="${PWD}/${basetmp}"
        cd "$oldpwd"
        echo "$result"
    fi
}

########
tok_corpus()
{
    # Obtain basenames
    srcbase=`"$BASENAME" "${scorpus_pref}"`
    trgbase=`"$BASENAME" "${tcorpus_pref}"`

    # Tokenize corpus
    echo "**** Tokenizing corpus" >&2
    suff="tok"

    "${bindir}"/thot_tokenize -f "${scorpus_train}" \
        > "${outd}/${preproc_dir}/${srcbase}_${suff}.train" 2>"${outd}/${preproc_dir}/thot_tokenize.log" || exit 1
    "${bindir}"/thot_tokenize -f "${scorpus_dev}" \
        > "${outd}/${preproc_dir}/${srcbase}_${suff}.dev" 2>>"${outd}/${preproc_dir}/thot_tokenize.log" || exit 1
    "${bindir}"/thot_tokenize -f ${scorpus_test} \
        > "${outd}/${preproc_dir}/${srcbase}_${suff}.test" 2>>"${outd}/${preproc_dir}/thot_tokenize.log" || exit 1
    "${bindir}"/thot_tokenize -f ${tcorpus_train} \
        > "${outd}/${preproc_dir}/${trgbase}_${suff}.train" 2>>"${outd}/${preproc_dir}/thot_tokenize.log" || exit 1
    "${bindir}"/thot_tokenize -f ${tcorpus_dev} \
        > "${outd}/${preproc_dir}/${trgbase}_${suff}.dev" 2>>"${outd}/${preproc_dir}/thot_tokenize.log" || exit 1
    "${bindir}"/thot_tokenize -f ${tcorpus_test} \
        > "${outd}/${preproc_dir}/${trgbase}_${suff}.test" 2>>"${outd}/${preproc_dir}/thot_tokenize.log" || exit 1
    echo "" >&2

    # Redefine corpus variables
    scorpus_train=${outd}/${preproc_dir}/${srcbase}_${suff}.train
    scorpus_dev=${outd}/${preproc_dir}/${srcbase}_${suff}.dev
    scorpus_test=${outd}/${preproc_dir}/${srcbase}_${suff}.test
    tcorpus_train=${outd}/${preproc_dir}/${trgbase}_${suff}.train
    tcorpus_dev=${outd}/${preproc_dir}/${trgbase}_${suff}.dev
    tcorpus_test=${outd}/${preproc_dir}/${trgbase}_${suff}.test
}

########
lowercase_corpus()
{
    # Obtain basenames
    srcbase=`"$BASENAME" "${scorpus_pref}"`
    trgbase=`"$BASENAME" "${tcorpus_pref}"`

    # Lowercase corpus
    echo "**** Lowercasing corpus" >&2
    if [ ${tok_given} -eq 0 ]; then
        suff="lc"
    else
        suff="tok_lc"
    fi

    "${bindir}"/thot_lowercase -f "${scorpus_train}" \
        > "${outd}/${preproc_dir}/${srcbase}_${suff}.train" 2>"${outd}/${preproc_dir}/thot_lowercase.log" || exit 1
    "${bindir}"/thot_lowercase -f "${scorpus_dev}" \
        > "${outd}/${preproc_dir}/${srcbase}_${suff}.dev" 2>>"${outd}/${preproc_dir}/thot_lowercase.log" || exit 1
    "${bindir}"/thot_lowercase -f "${scorpus_test}" \
        > "${outd}/${preproc_dir}/${srcbase}_${suff}.test" 2>>"${outd}/${preproc_dir}/thot_lowercase.log" || exit 1
    "${bindir}"/thot_lowercase -f "${tcorpus_train}" \
        > "${outd}/${preproc_dir}/${trgbase}_${suff}.train" 2>>"${outd}/${preproc_dir}/thot_lowercase.log" || exit 1
    "${bindir}"/thot_lowercase -f "${tcorpus_dev}" \
        > "${outd}/${preproc_dir}/${trgbase}_${suff}.dev" 2>>"${outd}/${preproc_dir}/thot_lowercase.log" || exit 1
    "${bindir}"/thot_lowercase -f "${tcorpus_test}" \
        > "${outd}/${preproc_dir}/${trgbase}_${suff}.test" 2>>"${outd}/${preproc_dir}/thot_lowercase.log" || exit 1
    echo "" >&2

    # Redefine corpus variables
    scorpus_train=${outd}/${preproc_dir}/${srcbase}_${suff}.train
    scorpus_dev=${outd}/${preproc_dir}/${srcbase}_${suff}.dev
    scorpus_test=${outd}/${preproc_dir}/${srcbase}_${suff}.test
    tcorpus_train=${outd}/${preproc_dir}/${trgbase}_${suff}.train
    tcorpus_dev=${outd}/${preproc_dir}/${trgbase}_${suff}.dev
    tcorpus_test=${outd}/${preproc_dir}/${trgbase}_${suff}.test
}

########
categ_corpus()
{
    # Obtain basenames
    srcbase=`$BASENAME "${scorpus_pref}"`
    trgbase=`$BASENAME "${tcorpus_pref}"`

    # Categorize corpus
    echo "**** Categorizing corpus" >&2
    suff=""
    if [ ${tok_given} -eq 1 ]; then
        suff="tok_"
    fi

    if [ ${lower_given} -eq 1 ]; then
        suff="${suff}lc_"
    fi

    suff="${suff}categ"

    "${bindir}"/thot_categorize -f "${scorpus_train}" \
        > "${outd}/${preproc_dir}/${srcbase}_${suff}.train" 2>"${outd}/${preproc_dir}/thot_categorize.log" || exit 1
    "${bindir}"/thot_categorize -f "${scorpus_dev}" \
        > "${outd}/${preproc_dir}/${srcbase}_${suff}.dev" 2>>"${outd}/${preproc_dir}/thot_categorize.log" || exit 1
    "${bindir}"/thot_categorize -f "${scorpus_test}" \
        > "${outd}/${preproc_dir}/${srcbase}_${suff}.test" 2>>"${outd}/${preproc_dir}/thot_categorize.log" || exit 1
    "${bindir}"/thot_categorize -f "${tcorpus_train}" \
        > "${outd}/${preproc_dir}/${trgbase}_${suff}.train" 2>>"${outd}/${preproc_dir}/thot_categorize.log" || exit 1
    "${bindir}"/thot_categorize -f "${tcorpus_dev}" \
        > "${outd}/${preproc_dir}/${trgbase}_${suff}.dev" 2>>"${outd}/${preproc_dir}/thot_categorize.log" || exit 1
    "${bindir}"/thot_categorize -f "${tcorpus_test}" \
        > "${outd}/${preproc_dir}/${trgbase}_${suff}.test" 2>>"${outd}/${preproc_dir}/thot_categorize.log" || exit 1
    echo "" >&2

    # Redefine corpus variables
    srctest_corpus_for_decat=${scorpus_test}
    scorpus_train=${outd}/${preproc_dir}/${srcbase}_${suff}.train
    scorpus_dev=${outd}/${preproc_dir}/${srcbase}_${suff}.dev
    scorpus_test=${outd}/${preproc_dir}/${srcbase}_${suff}.test
    tcorpus_train=${outd}/${preproc_dir}/${trgbase}_${suff}.train
    tcorpus_dev=${outd}/${preproc_dir}/${trgbase}_${suff}.dev
    tcorpus_test=${outd}/${preproc_dir}/${trgbase}_${suff}.test
}

########
clean_corpus()
{
    # Obtain basenames
    srcbase=`$BASENAME "${scorpus_pref}"`
    trgbase=`$BASENAME "${tcorpus_pref}"`

    # Clean corpus
    echo "**** Cleaning corpus" >&2
    suff=""
    if [ ${tok_given} -eq 1 ]; then
        suff="tok_"
    fi

    if [ ${lower_given} -eq 1 ]; then
        suff="${suff}lc_"
    fi

    if [ ${categ_given} -eq 1 ]; then
        suff="${suff}categ_"
    fi

    suff="${suff}clean"

    # Obtain line numbers of clean sentence pairs
    "${bindir}"/thot_clean_corpus_ln -s "${scorpus_train}" -t "${tcorpus_train}" \
        > "${outd}/${preproc_dir}/train_clean_ln" 2>"${outd}/${preproc_dir}/thot_clean_corpus_ln.log" || exit 1
    "${bindir}"/thot_clean_corpus_ln -s "${scorpus_dev}" -t "${tcorpus_dev}" \
        > "${outd}/${preproc_dir}/dev_clean_ln" 2>>"${outd}/${preproc_dir}/thot_clean_corpus_ln.log" || exit 1

    # Create file with clean sentence pairs
    "${bindir}"/thot_extract_sents_by_ln -n "${outd}/${preproc_dir}/train_clean_ln" \
        -f "${scorpus_train}" > "${outd}/${preproc_dir}/${srcbase}_${suff}.train" || exit 1
    "${bindir}"/thot_extract_sents_by_ln -n "${outd}/${preproc_dir}/dev_clean_ln" \
        -f "${scorpus_dev}" > "${outd}/${preproc_dir}/${srcbase}_${suff}.dev" || exit 1
    "${bindir}"/thot_extract_sents_by_ln -n "${outd}/${preproc_dir}/train_clean_ln" \
        -f "${tcorpus_train}" > "${outd}/${preproc_dir}/${trgbase}_${suff}.train" || exit 1
    "${bindir}"/thot_extract_sents_by_ln -n "${outd}/${preproc_dir}/dev_clean_ln" \
        -f "${tcorpus_dev}" > "${outd}/${preproc_dir}/${trgbase}_${suff}.dev" || exit 1

    echo "" >&2

    # Redefine corpus variables
    scorpus_train=${outd}/${preproc_dir}/${srcbase}_${suff}.train
    scorpus_dev=${outd}/${preproc_dir}/${srcbase}_${suff}.dev
    tcorpus_train=${outd}/${preproc_dir}/${trgbase}_${suff}.train
    tcorpus_dev=${outd}/${preproc_dir}/${trgbase}_${suff}.dev
}

########
decateg_output()
{
    echo "**** Decategorizing output" >&2

    # Define uncategorized source data file
    uncateg_src=`mktemp $tdir/uncateg_src.XXXXX`
    "${bindir}"/thot_get_srcsents_from_metadata -f "${srctest_corpus_for_decat}" > "${uncateg_src}" 2>/dev/null

    # Obtain alignment information
    alig_info_file=${outd}/output/${transoutd}/hyp_alig_info.txt
    "${bindir}"/thot_extract_hyp_info "${output_file}.dec_err" > "${alig_info_file}"

    # Decategorize output
    "${bindir}"/thot_decategorize -t "${output_file}" -s "${uncateg_src}" \
        -i "${alig_info_file}" \
        > "${output_file}_decateg" 2> "${outd}/output/${transoutd}/thot_decategorize.log" || exit 1
    echo "" >&2

    # Remove temporary files
    rm "${uncateg_src}"

    # Redefine output_file variable
    output_file=${output_file}_decateg
}

########
recase_output()
{
    echo "**** Recasing output" >&2

    # Determine basic raw files
    if [ ${tok_given} -eq 0 ]; then
        raw_src_pref=${scorpus_pref}
        raw_trg_pref=${tcorpus_pref}
    else
        raw_src_pref=${outd}/${preproc_dir}/${srcbase}_tok
        raw_trg_pref=${outd}/${preproc_dir}/${trgbase}_tok
    fi

    # Generate raw text file for recasing
    "${bindir}"/thot_gen_rtfile -s "${raw_src_pref}" -t "${raw_trg_pref}" \
        ${nolim_opt} -tdir "$tdir" > "$tdir/rfile_rec" 2> "${outd}/output/${transoutd}/thot_gen_rtfile_rec.log" || exit 1
      
    # Recase output
    "${bindir}"/thot_recase -f "${output_file}" -r "$tdir/rfile_rec" -w \
        -tdir "$tdir" > "${output_file}_rec" 2> "${outd}/output/${transoutd}/thot_recase.log" || exit 1
    echo "" >&2

    # Remove temporary files
    rm "$tdir/rfile_rec"

    # Redefine output_file variable
    output_file=${output_file}_rec
}

########
detok_output()
{
    echo "**** Detokenizing output" >&2

    # Generate raw text file for detokenizing
    "${bindir}"/thot_gen_rtfile -s "${scorpus_pref}" -t "${tcorpus_pref}" \
        ${nolim_opt} -tdir "$tdir" > "$tdir/rfile_detok" 2> "${outd}/output/${transoutd}/thot_gen_rtfile_detok.log" || exit 1

    # Detokenize output
    "${bindir}"/thot_detokenize -f "${output_file}" -r "$tdir/rfile_detok" \
        -tdir "$tdir" > "${output_file}_detok" 2> "${outd}/output/${transoutd}/thot_detokenize.log" || exit 1
    echo "" >&2

    # Remove temporary files
    rm "$tdir/rfile_detok"

    # Redefine output_file variable
    output_file=${output_file}_detok
}

########
process_pars()
{
    # Create preproc dir if necessary
    if [ ${tok_given} -eq 1 -o ${lower_given} -eq 1 -o ${skip_clean_given} -eq 0 ]; then
        # Store preproc dir name in a variable
        preproc_dir=preproc_data/initial
        # Check if the directory exists
        if [ ! -d "${outd}/${preproc_dir}" ]; then
            mkdir -p "${outd}/${preproc_dir}" || exit 1
        fi
    fi

    # Tokenize corpus if requested
    if [ ${tok_given} -eq 1 ]; then
        tok_corpus
    fi

    # Lowercase corpus if requested
    if [ ${lower_given} -eq 1 ]; then
        lowercase_corpus
    fi

    # Categorize corpus if requested
    if [ ${categ_given} -eq 1 ]; then
        categ_corpus
    fi

    # Clean corpus if requested
    if [ ${skip_clean_given} -eq 0 ]; then
        clean_corpus
    fi

    # Train models
    if [ -f "${scorpus_train}" -a -f "${tcorpus_train}" ]; then

        # Train language model
        echo "**** Training language model" >&2
        "${bindir}"/thot_lm_train -pr ${pr_val} -c "${tcorpus_train}" -o "${outd}/lm" -n ${n_val} -unk \
                 ${lmtype_opt} ${qs_opt} "${qs_par}" -tdir "$tdir" -sdir "$sdir" ${debug_opt} || exit 1
        
        # Train translation model
        echo "**** Training translation model" >&2
        "${bindir}"/thot_tm_train -pr ${pr_val} -s "${scorpus_train}" -t "${tcorpus_train}" -o "${outd}/tm" -nit ${nitval} \
                 -cpr ${cprval} -m ${m_val} ${ao_opt} ${tmtype_opt} ${qs_opt} "${qs_par}" -tdir "$tdir" -sdir "$sdir" ${debug_opt} || exit 1

    else
        echo "Error! training files do not exist" >&2
        exit 1                
    fi

    # Generate cfg file
    echo "**** Generating configuration file" >&2
    "${bindir}"/thot_gen_cfg_file "$outd/lm/lm_desc" "$outd/tm/tm_desc" > "$outd/before_tuning.cfg" || exit 1
    echo "" >&2

    # Tune parameters
    if [ -f "${scorpus_dev}" -a -f "${tcorpus_dev}" ]; then
        echo "**** Tuning model parameters" >&2
        "${bindir}"/thot_smt_tune -pr ${pr_val} -c "$outd/before_tuning.cfg" -s "${scorpus_dev}" -t "${tcorpus_dev}" -o "$outd/smt_tune" ${qs_opt} \
                 ${qs_opt} "${qs_par}" -tdir "$tdir" -sdir "$sdir" ${debug_opt} || exit 1
        tuning_executed="yes"
    fi

    # Translate test corpus if requested

    if [ ${notrans_given} -eq 0 ]; then

        # Create dir for model filtering
        if [ ! -d "${outd}/output/$curr_date" ]; then
            mkdir -p "${outd}/filtered_models" || exit 1
        fi

        # Obtain basename of ${scorpus_test}
        base_sct=`$BASENAME "${scorpus_test}"`

        # Prepare system to translate test corpus
        if [ -f "${scorpus_test}" -a -f "${tcorpus_test}" -a ${tuning_executed} = "yes" ]; then
            echo "**** Preparing system to translate test corpus" >&2
            "${bindir}"/thot_prepare_sys_for_test -c "$outd/smt_tune/tuned_for_dev.cfg" -t "${scorpus_test}"  \
                     -o "$outd/filtered_models/${base_sct}" ${qs_opt} "${qs_par}" -tdir "$tdir" -sdir "$sdir" ${debug_opt} || exit 1
            echo "" >&2
        fi

        # Obtain current date
        curr_date=`date '+%Y_%m_%d'`

        # Create variable containing traslator output dir name
        transoutd=${base_sct}.${curr_date}

        # Create translator output dir
        if [ ! -d "${outd}/output/${transoutd}" ]; then
            mkdir -p "${outd}/output/${transoutd}" || exit 1
        fi

        # Generate translations
        if [ -f "${scorpus_test}" -a -f "${tcorpus_test}" -a ${tuning_executed} = "yes" ]; then
            echo "**** Translating test corpus" >&2
            "${bindir}"/thot_decoder -pr ${pr_val} -c "$outd/filtered_models/${base_sct}/test_specific.cfg" \
                     -t "${scorpus_test}" -o "$outd/output/${transoutd}/thot_decoder_out" ${debug_opt} -sdir "$sdir" -v || exit 1
            test_trans_executed="yes"
            echo "" >&2
        fi

        # Obtain score given by thot_scorer
        if [ ${test_trans_executed} = "yes" ]; then
            echo "**** Obtaining thot_scorer score" >&2
            "${bindir}"/thot_scorer -r "${tcorpus_test}" -t "$outd/output/${transoutd}/thot_decoder_out" \
                     > "$outd/output/${transoutd}/thot_decoder_out.score" || exit 1
            echo "" >&2
        fi

        # Obtain BLEU score
        if [ ${test_trans_executed} = "yes" ]; then
            echo "**** Obtaining BLEU score" >&2
            "${bindir}"/thot_calc_bleu -r "${tcorpus_test}" -t "$outd/output/${transoutd}/thot_decoder_out" \
                     > "$outd/output/${transoutd}/thot_decoder_out.bleu" || exit 1
            echo "" >&2
        fi

        ### Execute post-processing steps if required

        # Define output_file variable
        output_file=$outd/output/${transoutd}/thot_decoder_out
        
        # Decategorizing stage
        if [ ${categ_given} -eq 1 ]; then
            # Decategorize
            decateg_output
        fi

        # Recasing stage
        if [ ${lower_given} -eq 1 ]; then
            # Recase
            recase_output
        fi

        # Detokenization stage
        if [ ${tok_given} -eq 1 ]; then
            # Detokenize
            detok_output
        fi

    else

        echo "Warning: translation step has been skipped because --no-trans option was provided" >&2
        
    fi
}

########
if [ $# -lt 1 ]; then
    print_desc
    exit 1
fi

# Read parameters
pr_given=0
pr_val=1
s_given=0
t_given=0
o_given=0
skip_clean_given=0
tok_given=0
lower_given=0
categ_given=0
nolim_given=0
notrans_given=0
n_given=0
n_val=4
nit_given=0
nitval=5
cpr_given=0
cprval=0.00001
m_val=10
ao_given=0
ao_opt="-ao sym1"
kenlm_given=0
ldblm_given=0
bdbtm_given=0
ldbtm_given=0
qs_given=0
tdir_given=0
tdir="/tmp"
sdir_given=0
sdir=$HOME
debug=0
debug_opt=""

while [ $# -ne 0 ]; do
    case $1 in
        "--help") usage
            exit 0
            ;;
        "--version") version
            exit 0
            ;;
        "-pr") shift
            if [ $# -ne 0 ]; then
                pr_val=$1
                pr_given=1
            fi
            ;;
        "-s") shift
            if [ $# -ne 0 ]; then
                scorpus_pref=$1
                s_given=1
            fi
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                tcorpus_pref=$1
                t_given=1
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                outd=$1
                o_given=1
            fi
            ;;
        "--skip-clean") skip_clean_given=1
            ;;
        "--tok") tok_given=1
            ;;
        "--lower") lower_given=1
            ;;
        "--categ") categ_given=1
            ;;
        "--no-lim") nolim_given=1
            nolim_opt="--no-lim"
            ;;
        "--no-trans") notrans_given=1
            ;;
        "-n") shift
            if [ $# -ne 0 ]; then
                n_val=$1
                n_given=1
            fi
            ;;
        "-nit") shift
            if [ $# -ne 0 ]; then
                nitval=$1
                nit_given=1
            fi
            ;;
        "-cpr") shift
            if [ $# -ne 0 ]; then
                cprval=$1
                cpr_given=1
            fi
            ;;
        "-m") shift
            if [ $# -ne 0 ]; then
                m_val=$1
                m_given=1
            fi
            ;;
        "-ao") shift
            if [ $# -ne 0 ]; then
                ao_opt="-ao $1"
                ao_given=1
            fi
            ;;
        "-kenlm") kenlm_given=1
                  lmtype_opt="-kenlm"
            ;;
        "-ldblm") ldblm_given=1
                  lmtype_opt="-ldb"
                  ;;
        "-bdbtm") bdbbtm_given=1
                  tmtype_opt="-bdb"
                  ;;
        "-ldbtm") ldbtm_given=1
                  tmtype_opt="-ldb"
                  ;;        
        "-qs") shift
            if [ $# -ne 0 ]; then
                qs_opt="-qs"
                qs_par="$1"
                qs_given=1
            else
                qs_given=0
            fi
            ;;
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1
                tdir_given=1
            fi
            ;;
        "-sdir") shift
            if [ $# -ne 0 ]; then
                sdir=$1
                sdir_given=1
            fi
            ;;
        "-debug") debug=1
            debug_opt="-debug"
            ;;
    esac
    shift
done

# Check parameters
if [ ${s_given} -eq 0 ]; then
    echo "Error! -s parameter not given" >&2
    exit 1
else
    # Obtain absolute path
    scorpus_pref=`get_absolute_path $scorpus_pref`

    # Complete prefix of source files
    scorpus_train=${scorpus_pref}.train
    scorpus_dev=${scorpus_pref}.dev
    scorpus_test=${scorpus_pref}.test

    # Check existence of files
    for file in "${scorpus_train}" "${scorpus_dev}" "${scorpus_test}"; do
        if [ ! -f "${file}" ]; then
            echo "Error! file ${file} does not exist" >&2
            exit 1
        fi
    done
fi

if [ ${t_given} -eq 0 ]; then        
    echo "Error! -t parameter not given" >&2
    exit 1
else
    # Obtain absolute path
    tcorpus_pref=`get_absolute_path $tcorpus_pref`

    # Complete prefix of target files
    tcorpus_train=${tcorpus_pref}.train
    tcorpus_dev=${tcorpus_pref}.dev
    tcorpus_test=${tcorpus_pref}.test

    # Check existence of files
    for file in "${tcorpus_train}" "${tcorpus_dev}" "${tcorpus_test}"; do
        if [ ! -f "${file}" ]; then
            echo "Error! file ${file} does not exist" >&2
            exit 1
        fi
    done

fi

# Check that source and target files are parallel
nl_source=`wc -l "$scorpus_train" | "$AWK" '{printf"%d",$1}'`
nl_target=`wc -l "$tcorpus_train" | "$AWK" '{printf"%d",$1}'`
if [ ${nl_source} -ne ${nl_target} ]; then
    echo "Error! source and target training files have not the same number of lines" >&2 
    exit 1
fi

nl_source=`wc -l "$scorpus_dev" | "$AWK" '{printf"%d",$1}'`
nl_target=`wc -l "$tcorpus_dev" | "$AWK" '{printf"%d",$1}'`
if [ ${nl_source} -ne ${nl_target} ]; then
    echo "Error! source and target development files have not the same number of lines" >&2 
    exit 1
fi

nl_source=`wc -l "$scorpus_test" | "$AWK" '{printf"%d",$1}'`
nl_target=`wc -l "$tcorpus_test" | "$AWK" '{printf"%d",$1}'`
if [ ${nl_source} -ne ${nl_target} ]; then
    echo "Error! source and target test files have not the same number of lines" >&2 
    exit 1
fi

if [ ${o_given} -eq 0 ]; then
    echo "Error! -o parameter not given!" >&2
    exit 1
else
    if [ -d ${outd} ]; then
        echo "Warning! output directory does exist" >&2 
    else
        # Create directory
        mkdir -p ${outd} || { echo "Error! cannot create output directory" >&2; exit 1; }
    fi
    # Obtain absolute path
    outd=`get_absolute_path "$outd"`
fi

if [ ${tdir_given} -eq 1 ]; then
    if [ ! -d "${tdir}" ]; then
        echo "Error! directory ${tdir} does not exist" >&2
        exit 1   
    fi         
fi

if [ ${sdir_given} -eq 1 ]; then
    if [ ! -d "${sdir}" ]; then
        echo "Error! directory ${sdir} does not exist" >&2
        exit 1            
    fi
fi

if [ ${kenlm_given} -eq 1 -a ${ldblm_given} -eq 1 ]; then
    echo "Error! -kenlm and -ldblm options cannot be given simultaneously" >&2
    exit 1   
fi

if [ ${bdbtm_given} -eq 1 -a ${ldbtm_given} -eq 1 ]; then
    echo "Error! -bdbtm and -ldbtm options cannot be given simultaneously" >&2
    exit 1   
fi

## Print parameters
echo "-pr is ${pr_val}" > "${outd}/input_pars.txt"
echo "-s is ${scorpus_pref}" >> "${outd}/input_pars.txt"
echo "-t is ${tcorpus_pref}" >> "${outd}/input_pars.txt"
echo "-o is ${outd}" >> "${outd}/input_pars.txt"
echo "-n is ${n_val}" >> "${outd}/input_pars.txt"
echo "-nit is ${nitval}" >> "${outd}/input_pars.txt"
echo "-cpr is ${cprval}" >> "${outd}/input_pars.txt"
echo "--skip-clean is ${skip_clean_given}" >> "${outd}/input_pars.txt"
echo "--tok is ${tok_given}" >> "${outd}/input_pars.txt"
echo "--lower is ${lower_given}" >> "${outd}/input_pars.txt"
echo "--categ is ${categ_given}" >> "${outd}/input_pars.txt"
echo "--no-lim is ${nolim_given}" >> "${outd}/input_pars.txt"
echo "-tdir is ${tdir}" >> "${outd}/input_pars.txt"
echo "-sdir is ${sdir}" >> "${outd}/input_pars.txt"

## Process parameters
process_pars
