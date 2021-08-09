# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Trains a phrase-based model given a parallel corpus.

# \textbf{Categ}: modelling

########
print_desc()
{
    echo "thot_lm_train written by Daniel Ortiz"
    echo "thot_lm_train trains a language model from a monolingual corpus"
    echo "type \"thot_lm_train --help\" to get usage information"
}

########
version()
{
    echo "thot_lm_train is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_lm_train      [-pr <int>] -c <string> [-o <string>|-a <string>]"
    echo "                   -n <int> [-f <int>] [-unk]"
    if [ "${KENLM_BUILD_DIR}" != "no" ]; then
        echo "                   [-kenlm]"
    fi
    if [ ! -z "${LEVELDB_LIB}" ]; then
        echo "                   [-ldb]"
    fi
    echo "                   [-qs <string>] [-tdir <string>] [-sdir <string>]"
    echo "                   [-debug] [--help] [--version]"
    echo ""
    echo "-pr <int>          Number of processors."
    echo "-c <string>        Corpus file."
    echo "-o <string>        Output directory common to all processors."
    echo "-a <string>        Output directory previously generated by thot_lm_train."
    echo "                   The new model will be added to models already generated."
    echo "-n <int>           Order of the n-grams."
    echo "-f <int>           Size in lines of the fragments in which the corpus"
    echo "                   is divided when performing map-reduce (50K by default)."
    echo "-unk               Reserve probability mass for the unknown word."
    if [ ! "${KENLM_BUILD_DIR}" = "no" ]; then
        echo "-kenlm             Generate on-disk language model in KenLM format."
    fi
    if [ ! -z "${LEVELDB_LIB}" ]; then
        echo "-ldb               Generate on-disk language model in LevelDB format."
    fi
    echo "-qs <string>       Specific options to be given to the qsub command"
    echo "                   (example: -qs \"-l pmem=1gb\")."
    echo "                   NOTES:"
    echo "                    a) ignore this if not using a PBS cluster"
    echo "                    b) -qs option may be crucial to ensure the correct"
    echo "                       execution of the tool. The main purpose of -qs"
    echo "                       is to reserve the required cluster resources."
    echo "                       If the necessary resources are not met the"
    echo "                       execution will abort."
    echo "                       Resources are reserved in different ways depending"
    echo "                       on the cluster software. For instance, if using SGE"
    echo "                       software, -qs \"-l h_vmem=1G,h_rt=10:00:00\","
    echo "                       requests 1GB of virtual memory and a time limit"
    echo "                       of 10 hours." 
    echo "-tdir <string>     Directory for temporary files (/tmp by default)."
    echo "                   NOTES:"
    echo "                    a) give absolute paths when using pbs clusters."
    echo "                    b) ensure there is enough disk space in the partition."
    echo "-sdir <string>     Absolute path of a directory common to all"
    echo "                   processors. If not given, \$HOME will be used."
    echo "                   NOTES:"
    echo "                    a) give absolute paths when using pbs clusters."
    echo "                    b) ensure there is enough disk space in the partition."
    echo "-debug             After ending, do not delete temporary files"
    echo "                   (for debugging purposes)."
    echo "--help             Display this help and exit."
    echo "--version          Output version information and exit."
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
    absolute=`is_absolute_path "$file"`
    if [ "$absolute" -eq 1 ]; then
        echo "$file"
    else
        oldpwd=$PWD
        basetmp=`$BASENAME "$PWD/$file"`
        dirtmp=`$DIRNAME "$PWD/$file"`
        cd "$dirtmp"
        result="${PWD}/${basetmp}"
        cd "$oldpwd"
        echo "$result"
    fi
}

########
generate_global_word_predictor_file()
{
    # Remove previous word predictor file
    if [ -f "${outd}/lm_desc.wp" ]; then
        rm "${outd}/lm_desc.wp"
    fi

    # Gather current word predictor files
    nlines_wp_file=100000
    tmpfile1=`"${MKTEMP}"`
    for file in "${outd}/main/*.wp" "${outd}/additional*/*.wp" ; do
        if [ -f "$file" ]; then
            cat "$file" >> "$tmpfile1" || return 1
        fi
    done

    # Generate global word predictor file
    tmpfile2=`"${MKTEMP}"`
    "${bindir}"/thot_shuffle 31415 "$tdir" "$tmpfile1" > "$tmpfile2" || return 1
    "$HEAD" -${nlines_wp_file} "$tmpfile2" > "${outd}/lm_desc.wp" || return 1
    rm "$tmpfile1" "$tmpfile2"
}

########
create_desc_files()
{
    # Determine model type
    if [ "${KENLM_BUILD_DIR}" != "no" -a ${kenlm_given} -eq 1 ]; then
        modeltype="\$(${LIBDIR_VARNAME})/kenlm_factory.so"
    else
        if [ ! -z "${LEVELDB_LIB}" -a ${ldb_given} -eq 1 ]; then
            modeltype="\$(${LIBDIR_VARNAME})/incr_jel_mer_leveldb_ngram_lm_factory.so"
        else
            modeltype="\$(${LIBDIR_VARNAME})/incr_jel_mer_ngram_lm_factory.so"
        fi
    fi

    # Create descriptor file and file with weights
    if [ ${o_given} -eq 1 ]; then
        # -o option was given
        echo "thot lm descriptor # tool: thot_lm_train" > "${outd}/lm_desc"
        echo "${modeltype} ${relative_prefix} main # corpus file: ${corpus}" >> "${outd}/lm_desc"

        # add new weight to descriptor weights file
        echo "1" > "${outd}/lm_desc.weights"
    else
        # -a option was given
        echo "${modeltype} ${relative_prefix} ${outsubdir} # corpus file: ${corpus}" >> "${outd}/lm_desc"        

        # add new weight to descriptor weights file
        echo "1" >> "${outd}/lm_desc.weights"
    fi

    # Create global word predictor file
    generate_global_word_predictor_file    
}

########
generate_outsubdir_name()
{
    if [ ${o_given} -eq 1 ]; then
        # -o option was given
        echo "main"
    else
        # -a option was given
        success=0
        for num in 1 2 3 4 5 6 7 8 9 10; do
            if [ ! -d "${outd}/additional_${num}" ]; then
                echo "additional_${num}"
                success=1
                break
            fi
        done

        if [ ${success} -eq 0 ]; then
            # Maximum number of models was exceeded
            echo "Error! a maximum of 10 additional models are allowed" >&2
            return 1
        fi
    fi
}

########
estimate_thotlm()
{
    # Determine output directory information
    prefix=$outd/${outsubdir}/trg.lm
    relative_prefix="${outsubdir}/trg.lm"

    # Obtain number of lines for input file
    nl=`"$WC" -l $corpus | "$AWK" '{printf"%s",$1}'`

    # Estimate n-gram model parameters
    if [ $nl -gt 0 ]; then
        "${bindir}"/thot_pbs_get_ngram_counts -pr ${pr_val} \
                 -c "$corpus" -o "$prefix" -n ${n_val} -f ${fragm_size} ${unk_opt} \
                 ${qs_opt} "${qs_par}" -tdir "$tdir" -sdir "$sdir" ${debug_opt} || return 1
    else
        ${bindir}/thot_get_ngram_counts -c "$corpus" \
                 -n ${n_val} > "$prefix" || return 1
    fi
}

########
estimate_klm()
{
    # Determine output directory information
    prefix="$outd/${outsubdir}/trg.lm"
    relative_prefix="${outsubdir}/trg.lm"
    
    # Estimate n-gram model parameters
    "${KENLM_BUILD_DIR}"/bin/lmplz -T "$tdir" -o ${n_val} --text $corpus > "${prefix}.arpa" 2> "${prefix}.arpa.log" || return 1
    "${KENLM_BUILD_DIR}"/bin/build_binary -T "$tdir" trie "${prefix}.arpa" "${prefix}" 2> "${prefix}.log" || return 1
}

########
remove_prev_ldb_files()
{
    if [ -d "$prefix" -o -f "$prefix" ]; then
        rm -rf "$prefix"
        echo "Warning: previously existing leveldb model files were found and removed" >&2
    fi
}

########
estimate_ldb()
{
    # Determine output directory of native thot language model
    thotlm_prefix="$outd/${outsubdir}/trg.thotlm"

    # Obtain number of lines for input file
    nl=`"$WC" -l $corpus | "$AWK" '{printf"%s",$1}'`

    # Estimate n-gram model parameters
    if [ $nl -gt 0 ]; then
        "${bindir}"/thot_pbs_get_ngram_counts -pr ${pr_val} \
                 -c "$corpus" -o "${thotlm_prefix}" -n ${n_val} -f ${fragm_size} ${unk_opt} \
                 ${qs_opt} "${qs_par}" -tdir "$tdir" -sdir "$sdir" ${debug_opt} || return 1
    else
        ${bindir}/thot_get_ngram_counts -c "$corpus" \
                 -n ${n_val} > "${thotlm_prefix}" || return 1
    fi

    # Determine output directory information
    prefix="$outd/${outsubdir}/trg.lm"
    relative_prefix="${outsubdir}/trg.lm"

    # Remove previously existing ldb files
    remove_prev_ldb_files
    
    # Create leveldb model
    cat "${thotlm_prefix}" | "${bindir}/thot_ngram_to_leveldb" -o "$prefix" 2> "${prefix}.ldb_err" || return 1

    # Remove native thot language model files
    rm "${thotlm_prefix}"*
}

########
estimate_ngram_parameters()
{
    # Determine model type
    if [ "${KENLM_BUILD_DIR}" != "no" -a ${kenlm_given} -eq 1 ]; then
        # Estimate KenLM model
        estimate_klm
    else
        if [ ! -z "${LEVELDB_LIB}" -a ${ldb_given} -eq 1 ]; then
            # Estimate LevelDB model
            estimate_ldb
        else
            # Estimate thot language model
            estimate_thotlm
        fi
    fi
}

########
generate_weight_file()
{
    n_buckets=3
    bsize=10
    "${bindir}"/thot_gen_init_file_with_jmlm_weights ${n_val} ${n_buckets} ${bsize} > "$prefix.weights" || return 1
}

########
generate_word_prediction_file()
{
    head_nlines=1000000
    nlines_wp_file=100000
    tmpfile=`"${MKTEMP}"`
    "$HEAD" -${head_nlines} "$corpus" | "${bindir}"/thot_shuffle 31415 "$tdir" > "$tmpfile" || return 1
    "$HEAD" -${nlines_wp_file} "$tmpfile" > "$prefix.wp" || return 1
    rm "$tmpfile"
}

########
if [ $# -lt 1 ]; then
    print_desc
    exit 1
fi

# Read parameters
pr_given=0
pr_val=1
c_given=0
o_given=0
a_given=0
n_given=0
f_given=0
fragm_size=50000
qs_given=0
unk_given=0
kenlm_given=0
ldb_given=0
tdir_given=0
tdir="/tmp"
sdir_given=0
sdir=$HOME
debug=0

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
        "-c") shift
            if [ $# -ne 0 ]; then
                corpus=$1
                c_given=1
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                outd=$1
                o_given=1
            fi
            ;;
        "-a") shift
            if [ $# -ne 0 ]; then
                outd=$1
                a_given=1
            fi
            ;;
        "-n") shift
            if [ $# -ne 0 ]; then
                n_val=$1
                n_given=1
            fi
            ;;
        "-f") shift
            if [ $# -ne 0 ]; then
                fragm_size=$1
                f_given=1
            else
                f_given=0
            fi
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
        "-unk") unk_given=1
            unk_opt="-unk"
            ;;
        "-kenlm") kenlm_given=1
            ;;
        "-ldb") ldb_given=1
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
if [ ${c_given} -eq 0 ]; then
    echo "Error! -c parameter not given" >&2
    exit 1
else
    if [ ! -f "${corpus}" ]; then
        echo "Error! file ${corpus} does not exist" >&2
        exit 1            
    else
        # Obtain absolute path
        corpus=`get_absolute_path "$corpus"`
    fi
fi

if [ ${o_given} -eq 0 -a ${a_given} -eq 0 ]; then
    echo "Error! -o or -a parameter not given!" >&2    
    exit 1
fi

if [ ${o_given} -eq 1 -a ${a_given} -eq 1 ]; then
    echo "Error! -o or -a parameter cannot be given simultaneously!" >&2    
    exit 1
fi

if [ ${o_given} -eq 1 ]; then
    if [ -d "${outd}/main" ]; then
        echo "Warning! output directory does exist" >&2
    fi
    # Obtain absolute path
    outd=`get_absolute_path "$outd"`
fi

if [ ${a_given} -eq 1 ]; then
    if [ ! -d "${outd}/main" ]; then
        echo "Error! previous model estimated with thot_lm_train does not exist" >&2 
        exit 1
    fi
    # Obtain absolute path
    outd=`get_absolute_path "$outd"`
fi

if [ ${n_given} -eq 0 ]; then
    echo "Error! -n parameter not given!" >&2
    exit 1
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

if [ ${kenlm_given} -eq 1 -a ${ldb_given} -eq 1 ]; then
    echo "Error! -kenlm and -ldb options cannot be given simultaneously" >&2
    exit 1   
fi

# Process parameters

# Obtain name of the output subdirectory where the language model will
# be stored
outsubdir=`generate_outsubdir_name` || exit 1

# Create output subdirectory
mkdir -p "${outd}/${outsubdir}" || { echo "Error! cannot create output directory" >&2; exit 1; }

echo "* Estimating n-gram model parameters... " >&2
estimate_ngram_parameters || exit 1
echo "" >&2

if [ ${kenlm_given} -eq 0 ]; then
    echo "* Generating weight file... " >&2
    generate_weight_file || exit 1
    echo "" >&2
fi

echo "* Generating file for word prediction... " >&2
generate_word_prediction_file || exit 1
echo "" >&2

echo "* Generating descriptor file... " >&2
create_desc_files "$outd" || exit 1
echo "" >&2
