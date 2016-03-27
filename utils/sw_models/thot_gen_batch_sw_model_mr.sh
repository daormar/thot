# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Generates a single-word model using batch learning. The training
# procedure maintains the space complexity of the training process under
# control by using the map reduce technique.

print_desc()
{
    echo "thot_gen_batch_sw_model_mr written by Daniel Ortiz"
    echo "thot_gen_batch_sw_model_mr estimates a single word model using map reduce"
    echo "type \"thot_gen_batch_sw_model_mr --help\" to get usage information"
}

version()
{
    echo "thot_gen_batch_sw_model_mr is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

usage()
{
    echo "thot_gen_batch_sw_model_mr -s <string> -t <string> [-l <string>] -o <string>"
    echo "                      -n <int> [-npr <int>] [-cpr <float>]"
    echo "                      [-lf <float>] [-af <float>]"
    echo "                      [-np <float>] [-c <int>] [-nsm]"
    echo "                      [-tdir <string>] [-debug] [--help] [--version]"
    echo ""
    echo "-s <string>        : File with source sentences."
    echo "-t <string>        : File with target sentences."
    echo "-l <string>        : Prefix of single word model to load at first iteration."
    echo "-o <string>        : Output prefix."
    echo "-n <int>           : Number of EM iterations."
    echo "-npr <int>         : n parameter used to prune lexical table."
    echo "-cpr <float>       : c parameter used to prune lexical table."
    echo "-lf <float>        : lf value (only for HMM models)."
    echo "-af <float>        : af value (only for HMM models)."
    echo "-np <float>        : np value (only for HMM models)."
    echo "-c <int>           : Set the size of the chunks."
    echo "-nsm               : Do not estimate sentence length model."
    echo "-tdir <string>     : Directory for temporary files."
    echo "-debug             : After ending, do not delete temporary files"
    echo "                     (for debugging purposes)."
    echo "--help             : Display this help and exit."
    echo "--version          : Output version information and exit."
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

set_tmp_dir()
{
    # Create TMP directory
    TMP="${tdir}/thot_gen_batch_sw_model_mr_${PPID}_$$"
    if [ ${debug} -eq 0 ]; then
        trap "rm -rf $TMP 2>/dev/null" EXIT
    fi
    mkdir $TMP || { echo "Error: temporary directory cannot be created" >&2 ; return 1; }

    # Create temporary subdirectories
    chunks_dir=$TMP/chunks
    init_model_dir=$TMP/init_model
    curr_tables_dir=$TMP/curr_tables
    models_per_chunk_dir=$TMP/models_per_chunk
    filtered_model_dir=$TMP/filtered_model
    slmodel_dir=$TMP/slmodel
    mkdir ${chunks_dir} || return 1
    mkdir ${init_model_dir} || return 1
    mkdir ${curr_tables_dir} || return 1
    mkdir ${models_per_chunk_dir} || return 1
    mkdir ${filtered_model_dir} || return 1
    mkdir ${slmodel_dir} || return 1

    # Function executed correctly
    return 0
}

split_input()
{
    echo "+++ Splitting input: ${srcf} ${trgf}..." >> $TMP/log
    ${SPLIT} -l ${chunk_size} ${srcf} ${chunks_dir}/src\_chunk\_ || exit 1
    ${SPLIT} -l ${chunk_size} ${trgf} ${chunks_dir}/trg\_chunk\_ || exit 1
}

estimate_slmodel()
{
    if [ ${nsm_given} -eq 1 ]; then
        echo "+++ Warning: no sentence length model will be estimated." >> $TMP/log
        echo "Geometric" > ${slmodel_dir}/model
    else
        echo "+++ Estimating sentence length model..." >> $TMP/log
        echo "+++ Estimating sentence length model..." >&2
        ${bindir}/thot_gen_wigauss_slen_model ${srcf} ${trgf} > ${slmodel_dir}/model
    fi
}

define_init_model_info()
{
    if [ ${l_given} -eq 0 ]; then
        # Create void corpus
        $TOUCH ${init_model_dir}/void_corpus
    
        # Generate model for void corpus
        ${bindir}/thot_gen_sw_model -s ${init_model_dir}/void_corpus -t ${init_model_dir}/void_corpus \
            ${lf_opt} ${af_opt} ${np_opt} -eb -n 1 -nl -o ${init_model_dir}/model > ${init_model_dir}/log 2>&1 ; pipe_fail || return 1
     
        # Add complete vocabularies
        ${bindir}/thot_get_swm_vocab ${srcf} "NULL UNKNOWN_WORD <UNUSED_WORD>" > ${init_model_dir}/model.svcb
        ${bindir}/thot_get_swm_vocab ${trgf} "NULL UNKNOWN_WORD <UNUSED_WORD>" > ${init_model_dir}/model.tvcb

        # Define init_model_pref variable
        init_model_pref=${init_model_dir}/model

        # Function executed correctly
        return 0
    else
        # Define init_model_pref variable
        init_model_pref=${l_val}

        # Function executed correctly
        return 0
    fi
}

get_model_information()
{
    alig_ext="none"

    for f in `ls ${init_model_pref}*`; do
        bname=$(basename "$f")
        extension="${bname##*.}"
        case ${extension} in
            "hmm_lexnd") lex_ext="hmm_lexnd"
                ;;
            "ibm_lexnd") lex_ext="ibm_lexnd"
                ;;
            "ibm2_alignd") alig_ext="ibm2_alignd"
                ;;
            "hmm_alignd") alig_ext="hmm_alignd"
                ;;
        esac
    done
}

determine_file_format()
{
    if [ "${TEXTPARS}" = "enabled" ]; then
        file_format="text"
    else
        file_format="binary"
    fi
}

sort_lex_counts_text()
{
    ${SORT} ${SORT_TMP} ${sortpars} -k1n -k2n
}

sort_alig_counts_text()
{
    case ${alig_ext} in
        "hmm_alignd") ${SORT} ${SORT_TMP} ${sortpars} -k1n -k2n -k3n
            ;;
        "ibm2_alignd")  ${SORT} ${SORT_TMP} ${sortpars} -k1n -k2n -k3n -k4n
            ;;
    esac
}

sort_counts()
{
    if [ ${file_format} = "text" ]; then
        sort_counts_text
    else
        sort_counts_bin
    fi
}

sort_counts_text()
{
    ${AWK} -v c=$chunk_id '{printf"%s %s\n",$0,c}' ${models_per_chunk_dir}/${out_chunk}.${lex_ext} | \
        sort_lex_counts_text > ${curr_tables_dir}/lex_counts_${out_chunk}
    if [ ${alig_ext} != "none" ]; then 
        ${AWK} -v c=$chunk_id '{printf"%s %s\n",$0,c}' ${models_per_chunk_dir}/${out_chunk}.${alig_ext} | \
            sort_alig_counts_text > ${curr_tables_dir}/alig_counts_${out_chunk}
    fi
}

sort_counts_bin()
{
    ${bindir}/thot_sort_bin_ilextable -l ${models_per_chunk_dir}/${out_chunk}.${lex_ext} > ${curr_tables_dir}/lex_counts_${out_chunk}

    if [ ${alig_ext} != "none" ]; then 
        case ${alig_ext} in
            "hmm_alignd") ${bindir}/thot_sort_bin_ihmmatable \
                -a ${models_per_chunk_dir}/${out_chunk}.${alig_ext} > ${curr_tables_dir}/alig_counts_${out_chunk}
                ;;
            "ibm2_alignd") ${bindir}/thot_sort_bin_iibm2atable \
                -a ${models_per_chunk_dir}/${out_chunk}.${alig_ext} > ${curr_tables_dir}/alig_counts_${out_chunk}
                ;;
        esac
    fi
}

proc_chunk()
{
    if [ $n -eq 1 ]; then
        # First iteration
        # Estimate model from chunk
        ${bindir}/thot_gen_sw_model -s ${chunks_dir}/${src_chunk} -t ${chunks_dir}/${trg_chunk} \
            -l ${init_model_pref} ${lf_opt} ${af_opt} ${np_opt} -eb -n 1 -nl \
            -o ${models_per_chunk_dir}/${out_chunk} ; pipe_fail || return 1
        if [ ${debug} -ne 0 -a "${file_format}" = "text" ]; then
            echo "Entries in initial table: "`wc -l ${models_per_chunk_dir}/${out_chunk}.${lex_ext} | $AWK '{printf"%s",$1}'` >> $TMP/log
        fi            
    else
        # Second iteration or greater
        
        # Create filtered model...
        create_filtered_model

        # Estimate model from chunk
        ${bindir}/thot_gen_sw_model -s ${chunks_dir}/${src_chunk} -t ${chunks_dir}/${trg_chunk} \
            -l ${filtered_model_dir}/model ${lf_opt} ${af_opt} ${np_opt} -eb -n 1 -nl \
            -o ${models_per_chunk_dir}/${out_chunk} 2>> ${models_per_chunk_dir}/${out_chunk}.log ; pipe_fail || return 1
    fi

    # Sort counts individually but do not append them
    sort_counts || return 1

    # Generate information useful for model filtering (required in the
    # first iteration only)
    if [ $n -eq 1 ]; then
        generate_filter_info || return 1
    fi

    # Remove model files for chunk
    if [ ${debug} -eq 0 ]; then
        rm ${models_per_chunk_dir}/${out_chunk}*
    fi

    return 0
}

append_lex_sorted_counts_text()
{
    ${SORT} ${SORT_TMP} ${sortpars} -k1n -k2n -m ${curr_tables_dir}/lex_counts_*
}

append_alig_sorted_counts_text()
{
    case ${alig_ext} in
        "hmm_alignd") ${SORT} ${SORT_TMP} ${sortpars} -k1n -k2n -k3n -m ${curr_tables_dir}/alig_counts_*
            ;;
        "ibm2_alignd")  ${SORT} ${SORT_TMP} ${sortpars} -k1n -k2n -k3n -k4n -m ${curr_tables_dir}/alig_counts_*
            ;;
    esac
}

merge_lex_counts()
{
    if [ ${file_format} = "text" ]; then
        merge_lex_counts_text
    else
        merge_lex_counts_bin
    fi
}

merge_lex_counts_text()
{
    # Append and merge lex sorted counts
    append_lex_sorted_counts_text | ${bindir}/thot_merge_text_ilextable -ns -T $TMP > ${curr_tables_dir}/merged_lex_counts || return 1

    # Delete lex sorted counts
    rm ${curr_tables_dir}/lex_counts_*
}

merge_lex_counts_bin()
{
    # Merge lex sorted counts
    ${bindir}/thot_merge_bin_ilextable ${curr_tables_dir}/lex_counts_* > ${curr_tables_dir}/merged_lex_counts || return 1

    # Delete lex sorted counts
    rm ${curr_tables_dir}/lex_counts_*
}

merge_alig_counts()
{
    if [ ${file_format} = "text" ]; then
        merge_alig_counts_text
    else
        merge_alig_counts_bin
    fi
}

merge_alig_counts_text()
{
    # Append and merge alig sorted counts
    case ${alig_ext} in
        "hmm_alignd") append_alig_sorted_counts_text | ${bindir}/thot_merge_text_ihmmatable -ns -T $TMP > ${curr_tables_dir}/merged_alig_counts || return 1
            ;;
        "ibm2_alignd") append_alig_sorted_counts_text | ${bindir}/thot_merge_text_iibm2atable -ns -T $TMP > ${curr_tables_dir}/merged_alig_counts || return 1
            ;;
    esac

    # Delete alig sorted counts
    if [ ${alig_ext} != "none" ]; then 
        rm ${curr_tables_dir}/alig_counts_*
    fi
}

merge_alig_counts_bin()
{
    # Merge alig sorted counts
    case ${alig_ext} in
        "hmm_alignd") ${bindir}/thot_merge_bin_ihmmatable ${curr_tables_dir}/alig_counts_* > ${curr_tables_dir}/merged_alig_counts
            ;;
        "ibm2_alignd") ${bindir}/thot_merge_bin_iibm2atable ${curr_tables_dir}/alig_counts_*  > ${curr_tables_dir}/merged_alig_counts
            ;;
    esac

    # Delete alig sorted counts
    if [ ${alig_ext} != "none" ]; then 
        rm ${curr_tables_dir}/alig_counts_*
    fi
}

generate_filter_info()
{
    if [ ${file_format} = "text" ]; then
        generate_filter_info_text
    else
        generate_filter_info_bin
    fi
}

generate_filter_info_text()
{
    $AWK '{printf"%s %s\n",$1,$2}' ${curr_tables_dir}/lex_counts_${out_chunk} > ${filtered_model_dir}/${chunk}_lex_model_info
}

generate_filter_info_bin()
{
    ${bindir}/thot_gen_bin_lex_filter_info -l ${curr_tables_dir}/lex_counts_${out_chunk} > ${filtered_model_dir}/${chunk}_lex_model_info
}

filter_lex_table()
{
    if [ ${file_format} = "text" ]; then
        filter_lex_table_text_alt
    else
        filter_lex_table_bin
    fi
}

filter_lex_table_text()
{
    local svocfile=${init_model_pref}.svcb
    local tvocfile=${init_model_pref}.tvcb
    local scorpus=${chunks_dir}/${src_chunk}
    local tcorpus=${chunks_dir}/${trg_chunk}
    local lextable=${curr_tables_dir}/merged_lex_counts
    ${bindir}/thot_filter_text_ilextable $svocfile $tvocfile $scorpus $tcorpus $lextable > ${filtered_model_dir}/model.${lex_ext}
    if [ ${debug} -ne 0 ]; then
        echo " - Entries in unfiltered table: "`wc -l ${lextable} | $AWK '{printf"%s",$1}'` >> $TMP/log
        echo " - Entries in filtered table: "`wc -l ${filtered_model_dir}/model.${lex_ext} | $AWK '{printf"%s",$1}'` >> $TMP/log
    fi
}

filter_lex_table_text_alt()
{
    local lextable=${curr_tables_dir}/merged_lex_counts
    $AWK -v filt_info_file=${filtered_model_dir}/${chunk}_lex_model_info \
    'BEGIN{
           getline <filt_info_file
           sword=$1
           tword=$2
          }
          {
           if(sword==$1 && tword==$2)
           {
            printf"%s\n",$0
            getline <filt_info_file
            sword=$1
            tword=$2
           }
          }' $lextable > ${filtered_model_dir}/model.${lex_ext} || return 1
    if [ ${debug} -ne 0 ]; then
        echo " - Entries in unfiltered table: "`wc -l ${lextable} | $AWK '{printf"%s",$1}'` >> $TMP/log
        echo " - Entries in filtered table: "`wc -l ${filtered_model_dir}/model.${lex_ext} | $AWK '{printf"%s",$1}'` >> $TMP/log
    fi
}

filter_lex_table_bin()
{
    local lextable=${curr_tables_dir}/merged_lex_counts
    local filt_info_file=${filtered_model_dir}/${chunk}_lex_model_info

    ${bindir}/thot_filter_bin_ilextable -l $lextable -f ${filt_info_file} >${filtered_model_dir}/model.${lex_ext}
}

create_filtered_model()
{
    # Copy basic initial model files
    if [ ! -f ${filtered_model_dir}/model.src ]; then
        for f in `ls ${init_model_pref}*`; do
            bname=$(basename "$f")
            extension="${bname##*.}"
            filename="${bname%.*}"
            if [ $extension != ${lex_ext} -a $extension != ${alig_ext} -a $extension != "src" -a $extension != "trg" ]; then
                cp $f ${filtered_model_dir}/model.${extension}
            fi
        done
        
        # Generate void .src and .trg files
        echo "" > ${filtered_model_dir}/model.src
        echo "" > ${filtered_model_dir}/model.trg
    fi

    # Filter complete lexical model given chunk
    echo "Filtering lexical table..." >> $TMP/log
    filter_lex_table || return 1

    # Copy current alignment file
    if [ ${chunk_id} -eq 1 -a ${alig_ext} != "none" ]; then 
        cp ${curr_tables_dir}/merged_alig_counts ${filtered_model_dir}/model.${alig_ext}
    fi
}

prune_lex_table()
{
    if [ ${file_format} = "text" ]; then
        prune_lex_table_text
    else
        prune_lex_table_bin
    fi    
}

prune_lex_table_text()
{
    ${bindir}/thot_prune_text_ilextable -n ${npr_val} -c ${cpr_val} \
        -t ${curr_tables_dir}/merged_lex_counts -T $TMP > ${output}.${lex_ext}
    if [ ${debug} -ne 0 ]; then
        echo " - Entries in original table: "`wc -l ${curr_tables_dir}/merged_lex_counts | $AWK '{printf"%s",$1}'` >> $TMP/log
        echo " - Entries in pruned table: "`wc -l ${output}.${lex_ext} | $AWK '{printf"%s",$1}'` >> $TMP/log
    fi
}

prune_lex_table_bin()
{
    ${bindir}/thot_prune_bin_ilextable -l ${curr_tables_dir}/merged_lex_counts -n ${npr_val} -c ${cpr_val} > ${output}.${lex_ext}
}

generate_final_model()
{
    # Copy basic files
    for f in `ls ${init_model_pref}*`; do
        local bname=$(basename "$f")
        local extension="${bname##*.}"
        local filename="${bname%.*}"
        if [ $extension != "slmodel" -a $extension != ${lex_ext} -a $extension != ${alig_ext} -a $extension != "src" -a $extension != "trg" ]; then
            cp $f ${output}.${extension}
        fi
        
        # Create void .src and .trg files
        echo "" > ${output}.src
        echo "" > ${output}.trg
    done

    # Copy sentence length model
    cp ${slmodel_dir}/model ${output}.slmodel

    # Prune lexical table
    if [ ${npr_val} -eq 0 -a ${cpr_val} = "0" ]; then
        cp ${curr_tables_dir}/merged_lex_counts ${output}.${lex_ext}
    else
        # Prune lexical table
        echo "++ [Map-Reduce] Pruning lexical table..." >> $TMP/log
        echo "++ [Map-Reduce] Pruning lexical table..." >&2
        prune_lex_table || exit 1
    fi
    
    # Copy alignment table if exists
    if [ ${alig_ext} != "none" ]; then
        cp ${curr_tables_dir}/merged_alig_counts ${output}.${alig_ext}
    fi
}

# main

s_given=0
t_given=0
l_given=0
o_given=0
n_given=0
npr_given=0
npr_val=0
cpr_given=0
cpr_val=0
niters=1
lf_given=0
af_given=0
np_given=0
nsm_given=0
tdir="./"
debug=0
chunk_size=250000

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
        "-s") shift
            if [ $# -ne 0 ]; then
                srcf=$1
                s_given=1
            else
                s_given=0
            fi
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                trgf=$1
                t_given=1
            else
                t_given=0
            fi
            ;;
        "-l") shift
            if [ $# -ne 0 ]; then
                l_val=$1
                l_given=1
            else
                l_given=0
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                output=$1
                o_given=1
            else
                o_given=0
            fi
            ;;
        "-n") shift
            if [ $# -ne 0 ]; then
                niters=$1
                n_given=1
            else
                n_given=0
            fi
            ;;
        "-npr") shift
            if [ $# -ne 0 ]; then
                npr_val=$1
                npr_given=1
            else
                npr_given=0
            fi
            ;;
        "-cpr") shift
            if [ $# -ne 0 ]; then
                cpr_val=$1
                cpr_given=1
            else
                cpr_given=0
            fi
            ;;
        "-lf") shift
            if [ $# -ne 0 ]; then
                lf_opt="-lf $1"
                lf_given=1
            else
                lf_given=0
            fi
            ;;
        "-af") shift
            if [ $# -ne 0 ]; then
                af_opt="-af $1"
                af_given=1
            else
                af_given=0
            fi
            ;;
        "-np") shift
            if [ $# -ne 0 ]; then
                np_opt="-np $1"
                np_given=1
            else
                np_given=0
            fi
            ;;
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1
            else
                tdir="./"
            fi
            ;;
        "-c") shift
            if [ $# -ne 0 ]; then
                chunk_size=$1
            fi
            ;;
        "-nsm") nsm_given=1
            ;;
        "-debug") debug=1
            ;;
    esac
    shift
done

# verify parameters

if [ ${s_given} -eq 0 ]; then
    echo "Error: file with source sentences not given" >&2
    exit 1
else
    if [ ! -f  "${srcf}" ]; then
        echo "Error: file ${srcf} with source sentences does not exist" >&2
        exit 1
    fi
fi

if [ ${t_given} -eq 0 ]; then
    echo "Error: file with target sentences not given" >&2
    exit 1
else
    if [ ! -f  "${trgf}" ]; then
        echo "Error: file ${trgf} with target sentences does not exist" >&2
    fi
fi

if [ ${o_given} -eq 0 ];then
    # invalid parameters 
    echo "Error: output files prefix must be given" >&2
    exit 1
fi

if [ ${n_given} -eq 0 ]; then
    # invalid parameters 
    echo "Error: number of EM iterations must be given" >&2
    exit 1
fi

# parameters are ok

# Set TMP directory (global variables are declared)
declare chunks_dir="" 
declare init_model_dir="" 
declare curr_tables_dir="" 
declare models_per_chunk_dir=""
declare filtered_model_dir="" 
declare slmodel_dir=""

set_tmp_dir || exit 1

echo "++++ Process started at: " `date` > $TMP/log

# Determine file format
declare file_format=""
determine_file_format

# Define initial model information
define_init_model_info || exit 1

# Get model information (extracted from the initial model files)
declare lex_ext=""
declare alig_ext=""
get_model_information

# Split input into chunks and process them separately
split_input

# Estimate sentence length model
estimate_slmodel

# EM algorithm iterations
n=1
while [ $n -le ${niters} ]; do
    echo "+++ Map-reduce iter ${n}" >> $TMP/log
    echo "+++ Map-reduce iter ${n}" >&2

    chunk_id=0

    for i in `ls ${chunks_dir}/src\_chunk\_*`; do
        # Initialize variables
        chunk=`${BASENAME} $i`
        chunk=${chunk:4}
        src_chunk="src_"${chunk}
        trg_chunk="trg_"${chunk}
        out_chunk="out_"${chunk}
        chunk_id=`expr $chunk_id + 1`

        echo "++ [Map-Reduce] Processing chunk ${chunk}" >> $TMP/log
        echo "++ [Map-Reduce] Processing chunk ${chunk}" >&2

        # Process chunk
        proc_chunk || exit 1
    done

    # Merge counts for submodels
    echo "++ [Map-Reduce] Merging counts..." >> $TMP/log
    echo "++ [Map-Reduce] Merging counts..." >&2
    
    # Merge lexical counts
    merge_lex_counts || exit 1

    # Merge alignment counts
    merge_alig_counts || exit 1

    # Increase n (num iter)
    n=`expr $n + 1`
done

# Generate final model...
generate_final_model

# Copy log file
echo "++++ Process finished at: " `date` >> $TMP/log
cp $TMP/log ${output}.log
