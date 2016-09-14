# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Generates a single-word model using a pbs cluster.

print_desc()
{
    echo "thot_pbs_gen_batch_sw_model written by Daniel Ortiz"
    echo "thot_pbs_gen_batch_sw_model estimates a single word model using a pbs cluster"
    echo "type \"thot_pbs_gen_batch_sw_model --help\" to get usage information"
}

version()
{
    echo "thot_pbs_gen_batch_sw_model is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

usage()
{
    echo "thot_pbs_gen_batch_sw_model -pr <int>"
    echo "                       -s <string> -t <string> -o <string>"
    echo "                       -n <int> [-npr <int>] [-cpr <float>]"
    echo "                       [-lf <float>] [-af <float>]"
    echo "                       [-np <float>] [-lc <int>] [-shu] [-qs <string>]"
    echo "                       [-tdir <string>] [-sdir <string>]"
    echo "                       [--sync-dep] [-debug] [--help] [--version]"
    echo ""
    echo "-pr <int>          : Number of processors."
    echo "-s <string>        : File with source sentences (give absolute path when"
    echo "                     using pbs clusters)."
    echo "-t <string>        : File with target sentences (give absolute path when"
    echo "                     using pbs clusters)."
    echo "-o <out_pref>      : Output prefix (give absolute path when"
    echo "                     using pbs clusters)."
    echo "-n <int>           : Number of EM iterations."
    echo "-npr <int>         : n parameter used to prune lexical table."
    echo "-cpr <float>       : c parameter used to prune lexical table."
    echo "-lf <float>        : lf value (only for HMM models)."
    echo "-af <float>        : af value (only for HMM models)."
    echo "-np <float>        : np value (only for HMM models)."
    echo "-lc <int>          : Set the local chunk size for each processor."
    echo "-shu               : Shuffle input files before splitting them."
    echo "-qs <string>       : Specific options to be given to the qsub command"
    echo "                     (example: -qs \"-l pmem=1gb\")."
    echo "-tdir <string>     : Directory for temporary files (/tmp by default)."
    echo "                     NOTES:"
    echo "                      a) give absolute paths when using pbs clusters."
    echo "                      b) ensure there is enough disk space in the partition."
    echo "-sdir <string>     : Absolute path of a directory common to all"
    echo "                     processors. If not given, \$HOME will be used."
    echo "                     NOTES:"
    echo "                      a) give absolute paths when using pbs clusters."
    echo "                      b) ensure there is enough disk space in the partition."
    echo "--sync-dep         : Use qsub-defined job dependencies to synchronize processes"
    echo "                     (only for pbs clusters). Currently, the implementation"
    echo "                     still has portability issues."
    echo "-debug             : After ending, do not delete temporary files"
    echo "                     (for debugging purposes)."
    echo "--help             : Display this help and exit."
    echo "--version          : Output version information and exit."
}

disabled_pipe_fail()
{
    return $?
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
    if [ -d ${tdir} ]; then
        TMP=${tdir}
    else
        echo "Error: temporary directory does not exist" >&2
        return 1;
    fi
}

set_shared_dir()
{
    if [ ! -d ${sdir} ]; then
        echo "Error: shared directory does not exist" >&2
        return 1;
    fi

    SDIR="${sdir}/thot_pbs_gen_batch_sw_model_sdir_${PPID}_$$"
    mkdir $SDIR || { echo "Error: shared directory cannot be created" >&2 ; return 1; }

    # Create temporary subdirectories
    chunks_dir=$SDIR/chunks
    init_model_dir=$SDIR/init_model
    curr_tables_dir=$SDIR/curr_tables
    models_per_chunk_dir=$SDIR/models_per_chunk
    filtered_model_dir=$SDIR/filtered_model
    slmodel_dir=$SDIR/slmodel
    scripts_dir=$SDIR/scripts
    used_scripts_dir=$SDIR/used_scripts
    sync_info_dir=$SDIR/sync
    mkdir ${chunks_dir} || return 1
    mkdir ${init_model_dir} || return 1
    mkdir ${curr_tables_dir} || return 1
    mkdir ${models_per_chunk_dir} || return 1
    mkdir ${filtered_model_dir} || return 1
    mkdir ${slmodel_dir} || return 1
    mkdir ${scripts_dir} || return 1
    mkdir ${used_scripts_dir} || return 1
    mkdir ${sync_info_dir} || return 1

    # Function executed correctly
    return 0
}

split_input()
{
    echo "*** Shuffling and splitting input: ${srcf} ${trgf}..." >> $SDIR/log

    # Determine fragment size
    local input_size=`wc -l ${srcf} 2>/dev/null | ${AWK} '{printf"%d",$1}'`
    if [ ${input_size} -eq 0 ]; then
        echo "Error: input file ${srcf} is empty" >&2
        exit 1
    fi

    if [ ${input_size} -lt ${pr_val} ]; then
        echo "Error: problem too small" >&2
        exit 1
    fi
    local chunk_size=`expr ${input_size} / ${pr_val}`
    local chunk_size=`expr ${chunk_size} + 1`

    # Shuffle (optional) and split input (shuffling is required to
    # achieve load balancing)
    if [ ${shu_given} -eq 0 ]; then
        ${SPLIT} -l ${chunk_size} ${srcf} ${chunks_dir}/src\_chunk\_ || return 1
        ${SPLIT} -l ${chunk_size} ${trgf} ${chunks_dir}/trg\_chunk\_ || return 1
    else
        local rand_seed=31415
        ${bindir}/thot_shuffle ${rand_seed} ${tdir} ${srcf} | ${SPLIT} -l ${chunk_size} - ${chunks_dir}/src\_chunk\_ || return 1
        ${bindir}/thot_shuffle ${rand_seed} ${tdir} ${trgf} | ${SPLIT} -l ${chunk_size} - ${chunks_dir}/trg\_chunk\_ || return 1
    fi
}

estimate_slmodel()
{
    echo "*** Estimating sentence length model..." >> $SDIR/log
    echo "*** Estimating sentence length model..." >> ${slmodel_dir}/log

    ${bindir}/thot_gen_wigauss_slen_model ${srcf} ${trgf} > ${slmodel_dir}/model || \
        { echo "Error while executing estimate_slmodel" >> $SDIR/log ; return 1 ; }

    # Create sync file
    echo "" > ${sync_info_dir}/estimate_slmodel
}

estimate_init_model()
{
    # Create void corpus
    $TOUCH ${init_model_dir}/void_corpus
    
    # Generate model for void corpus
    ${bindir}/thot_gen_sw_model -s ${init_model_dir}/void_corpus -t ${init_model_dir}/void_corpus \
        ${lf_opt} ${af_opt} ${np_opt} -eb -n 1 -nl -o ${init_model_dir}/model 2> ${init_model_dir}/log || return 1
     
    # Add complete vocabularies
    ${bindir}/thot_get_swm_vocab ${srcf} "NULL UNKNOWN_WORD <UNUSED_WORD>" > ${init_model_dir}/model.svcb
    ${bindir}/thot_get_swm_vocab ${trgf} "NULL UNKNOWN_WORD <UNUSED_WORD>" > ${init_model_dir}/model.tvcb

    # Create msinfo file
    echo "0" > ${init_model_dir}/model.msinfo
    echo "0" >> ${init_model_dir}/model.msinfo

    # Function executed correctly
    return 0
}

get_model_information()
{
    export alig_ext="none"

    for f in `ls ${init_model_dir}/model*`; do
        bname=$(basename "$f")
        extension="${bname##*.}"
        case ${extension} in
            "hmm_lexnd") export lex_ext="hmm_lexnd"
                ;;
            "ibm_lexnd") export lex_ext="ibm_lexnd"
                ;;
            "ibm2_alignd") export alig_ext="ibm2_alignd"
                ;;
            "hmm_alignd") export alig_ext="hmm_alignd"
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
    LC_ALL=C ${SORT} ${SORT_TMP} ${sortpars} -k1n -k2n
}

sort_alig_counts_text()
{
    case ${alig_ext} in
        "hmm_alignd") LC_ALL=C ${SORT} ${SORT_TMP} ${sortpars} -k1n -k2n -k3n
            ;;
        "ibm2_alignd") LC_ALL=C ${SORT} ${SORT_TMP} ${sortpars} -k1n -k2n -k3n -k4n
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
    # Write date to log file
    echo "** Processing chunk ${chunk} (started at "`date`")..." >> $SDIR/log
    echo "** Processing chunk ${chunk} (started at "`date`")..." >> ${models_per_chunk_dir}/${out_chunk}_proc_n$n.log

    if [ $n -eq 1 ]; then
        # First iteration
        # Estimate model from chunk
        echo "* Estimate model for chunk ${chunk} (started at "`date`")..." >> $SDIR/log
        ${bindir}/thot_gen_batch_sw_model_mr -s ${chunks_dir}/${src_chunk} -t ${chunks_dir}/${trg_chunk} \
            -l ${init_model_dir}/model ${lf_opt} ${af_opt} ${np_opt} -n 1 -npr ${npr_val} \
            -cpr ${cpr_val} -c ${local_ch_size} -nsm -tdir $TMP \
            -o ${models_per_chunk_dir}/${out_chunk} 2>> ${models_per_chunk_dir}/${out_chunk}_proc_n$n.log || \
            { echo "Error while executing proc_chunk for ${chunk}" >> $SDIR/log ; return 1 ; }

        if [ ${debug} -ne 0 -a "${file_format}" = "text" ]; then
            echo "Entries in initial table (${chunk}): "`wc -l ${models_per_chunk_dir}/${out_chunk}.${lex_ext} | $AWK '{printf"%s",$1}'` >> $SDIR/log
        fi
    else
        # Second iteration or greater

        # Estimate model from chunk
        echo "* Estimate model for chunk ${chunk} (started at "`date`")..." >> $SDIR/log
        ${bindir}/thot_gen_batch_sw_model_mr -s ${chunks_dir}/${src_chunk} -t ${chunks_dir}/${trg_chunk} \
            -l ${filtered_model_dir}/${chunk}/model ${lf_opt} ${af_opt} ${np_opt} -n 1 \
            -npr ${npr_val} -cpr ${cpr_val} -c ${local_ch_size} -nsm -tdir $TMP \
            -o ${models_per_chunk_dir}/${out_chunk} 2>> ${models_per_chunk_dir}/${out_chunk}_proc_n$n.log || \
            { echo "Error while executing proc_chunk for ${chunk}" >> $SDIR/log ; return 1 ; }

    fi

    # Sort counts individually but do not append them
    echo "* Sort counts for chunk ${chunk} (started at "`date`")..." >> $SDIR/log
    sort_counts || { echo "Error while executing proc_chunk for ${chunk}" >> $SDIR/log ; return 1 ; }

    # Generate information useful for model filtering 
    generate_filter_info || { echo "Error while executing proc_chunk for ${chunk}" >> $SDIR/log ; return 1 ; }

    # Remove model files for chunk (except the log file)
    if [ ${debug} -eq 0 ]; then
        for file in ${models_per_chunk_dir}/${out_chunk}*; do
            log_ext=`echo $file | $AWK '{print substr($1,length($1)-3)==".log"}'`
            if [ ${log_ext} -eq 0 ]; then
                rm $file
            fi
        done
    fi

    # Write date to log file
    echo "Processing of chunk ${chunk} finished ("`date`")" >> $SDIR/log 

    # Create sync file
    echo "" > ${sync_info_dir}/proc_chunk_${chunk}

    return 0
}

append_lex_sorted_counts_text()
{
    LC_ALL=C ${SORT} ${SORT_TMP} ${sortpars} -k1n -k2n -m ${curr_tables_dir}/lex_counts_*
}

append_alig_sorted_counts_text()
{
    case ${alig_ext} in
        "hmm_alignd") LC_ALL=C ${SORT} ${SORT_TMP} ${sortpars} -k1n -k2n -k3n -m ${curr_tables_dir}/alig_counts_*
            ;;
        "ibm2_alignd") LC_ALL=C ${SORT} ${SORT_TMP} ${sortpars} -k1n -k2n -k3n -k4n -m ${curr_tables_dir}/alig_counts_*
            ;;
    esac
}

merge_lex_counts()
{
    echo "** Merging lex model counts (started at "`date`")..." >> ${curr_tables_dir}/merge_lex_n$n.log

    if [ ${file_format} = "text" ]; then
        merge_lex_counts_text 2>> ${curr_tables_dir}/merge_lex_n$n.log || \
            { echo "Error while executing merge_lex_counts" >> $SDIR/log ; return 1 ; }
    else
        merge_lex_counts_bin 2>> ${curr_tables_dir}/merge_lex_n$n.log || \
            { echo "Error while executing merge_lex_counts" >> $SDIR/log ; return 1 ; }
    fi

    # Create sync file
    echo "" > ${sync_info_dir}/merge_lex_counts
}

merge_lex_counts_text()
{
    # Append and merge lex sorted counts
    append_lex_sorted_counts_text | ${bindir}/thot_merge_text_ilextable -ns -T $TMP \
        > ${curr_tables_dir}/merged_lex_counts ; ${PIPE_FAIL} || return 1

    # Delete lex sorted counts
    rm ${curr_tables_dir}/lex_counts_*
}

merge_lex_counts_bin()
{
    # Merge lex sorted counts
    ${bindir}/thot_merge_bin_ilextable ${curr_tables_dir}/lex_counts_* \
        > ${curr_tables_dir}/merged_lex_counts ; ${PIPE_FAIL} || return 1

    # Delete lex sorted counts
    rm ${curr_tables_dir}/lex_counts_*
}

merge_alig_counts()
{
    echo "** Merging alig model counts (started at "`date`")..." >> ${curr_tables_dir}/merge_alig_n$n.log

    if [ ${file_format} = "text" ]; then
        merge_alig_counts_text 2>> ${curr_tables_dir}/merge_alig_n$n.log || \
            { echo "Error while executing merge_alig_counts" >> $SDIR/log ; return 1 ; }
    else
        merge_alig_counts_bin 2>> ${curr_tables_dir}/merge_alig_n$n.log || \
            { echo "Error while executing merge_alig_counts" >> $SDIR/log ; return 1 ; }
    fi

    # Create sync file
    echo "" > ${sync_info_dir}/merge_alig_counts
}

merge_alig_counts_text()
{
    # Append and merge alig sorted counts
    case ${alig_ext} in
        "hmm_alignd") append_alig_sorted_counts_text | ${bindir}/thot_merge_text_ihmmatable -ns -T $TMP \
            > ${curr_tables_dir}/merged_alig_counts ; ${PIPE_FAIL} || return 1
            ;;
        "ibm2_alignd") append_alig_sorted_counts_text | ${bindir}/thot_merge_text_iibm2atable -ns -T $TMP \
            > ${curr_tables_dir}/merged_alig_counts ; ${PIPE_FAIL} || return 1
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
        "hmm_alignd") ${bindir}/thot_merge_bin_ihmmatable ${curr_tables_dir}/alig_counts_* \
            > ${curr_tables_dir}/merged_alig_counts ; ${PIPE_FAIL} || return 1
            ;;
        "ibm2_alignd") ${bindir}/thot_merge_bin_iibm2atable ${curr_tables_dir}/alig_counts_* \
            > ${curr_tables_dir}/merged_alig_counts ; ${PIPE_FAIL} || return 1
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
    local svocfile=${init_model_dir}/model.svcb
    local tvocfile=${init_model_dir}/model.tvcb
    local scorpus=${chunks_dir}/${src_chunk}
    local tcorpus=${chunks_dir}/${trg_chunk}
    local lextable=${curr_tables_dir}/merged_lex_counts
    ${bindir}/thot_filter_text_ilextable $svocfile $tvocfile $scorpus $tcorpus $lextable > ${chunk_filtered_model_dir}/model.${lex_ext}
    if [ ${debug} -ne 0 ]; then
        echo "Entries in unfiltered table (${chunk}): "`wc -l ${lextable} | $AWK '{printf"%s",$1}'` >> $SDIR/log
        echo "Entries in filtered table (${chunk}): "`wc -l ${chunk_filtered_model_dir}/model.${lex_ext} | $AWK '{printf"%s",$1}'` >> $SDIR/log
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
          }' $lextable > ${chunk_filtered_model_dir}/model.${lex_ext}
    if [ ${debug} -ne 0 ]; then
        echo "Entries in unfiltered table (${chunk}): "`wc -l ${lextable} | $AWK '{printf"%s",$1}'` >> $SDIR/log
        echo "Entries in filtered table (${chunk}): "`wc -l ${chunk_filtered_model_dir}/model.${lex_ext} | $AWK '{printf"%s",$1}'` >> $SDIR/log
    fi
}

filter_lex_table_bin()
{
    lextable=${curr_tables_dir}/merged_lex_counts
    filt_info_file=${filtered_model_dir}/${chunk}_lex_model_info

    ${bindir}/thot_filter_bin_ilextable -l $lextable -f ${filt_info_file} > ${chunk_filtered_model_dir}/model.${lex_ext}
}

create_filtered_model()
{
    echo "** Creating filtered model for chunk ${chunk} (started at "`date`")..." >> ${filtered_model_dir}/${chunk}_filt_n$n.log

    # Define directory name to store filtered model
    chunk_filtered_model_dir=${filtered_model_dir}/${chunk}

    # Create directory if necessary
    if [ ! -d ${chunk_filtered_model_dir} ]; then
        mkdir ${chunk_filtered_model_dir}
    fi

    # Copy basic files
    if [ ! -f ${chunk_filtered_model_dir}/model.src ]; then
        # Copy void model files
        cp ${init_model_dir}/model* ${chunk_filtered_model_dir} || \
        { echo "Error while executing create_filtered_model" >> $SDIR/log ; return 1 ; }
    fi

    # Filter complete lexical model given chunk
    echo "* Filtering lexical table (${chunk})..." >> $SDIR/log
    filter_lex_table 2>> ${filtered_model_dir}/${chunk}_filt_n$n.log || \
        { echo "Error while executing create_filtered_model" >> $SDIR/log ; return 1 ; }

    # Copy current alignment file
    cp ${curr_tables_dir}/merged_alig_counts ${chunk_filtered_model_dir}/model.${alig_ext} || \
        { echo "Error while executing create_filtered_model" >> $SDIR/log ; return 1 ; }

    # Create sync file
    echo "" > ${sync_info_dir}/create_filtered_model_${chunk}
}


prune_lex_table()
{
    echo "*** Pruning lexical table (started at "`date`")..." >> $SDIR/log

    if [ ${file_format} = "text" ]; then
        prune_lex_table_text || return 1
    else
        prune_lex_table_bin || return 1
    fi    
}

prune_lex_table_text()
{
    ${bindir}/thot_prune_text_ilextable -n ${npr_val} -c ${cpr_val} \
        -t ${curr_tables_dir}/merged_lex_counts -T $TMP > ${output}.${lex_ext} || return 1
    if [ ${debug} -ne 0 ]; then
        echo "Entries in original table: "`wc -l ${curr_tables_dir}/merged_lex_counts | $AWK '{printf"%s",$1}'` >> $SDIR/log
        echo "Entries in pruned table: "`wc -l ${output}.${lex_ext} | $AWK '{printf"%s",$1}'` >> $SDIR/log
    fi

}

prune_lex_table_bin()
{
    ${bindir}/thot_prune_bin_ilextable -l ${curr_tables_dir}/merged_lex_counts -n ${npr_val} -c ${cpr_val} \
        > ${output}.${lex_ext} || return 1
}

generate_final_model()
{
    echo "*** Copying final model (started at "`date`")..." >> $SDIR/log
    echo "*** Copying final model (started at "`date`")..." >> ${curr_tables_dir}/generate_final_model.log

    for f in `ls ${init_model_dir}/model*`; do
        # Copy basic files
        bname=$(basename "$f")
        extension="${bname##*.}"
        filename="${bname%.*}"
        cp $f ${output}.${extension} || \
            { echo "Error while executing generate_final_model" >> $SDIR/log ; return 1 ; }
    done
    
    # Copy sentence length model
    cp ${slmodel_dir}/model ${output}.slmodel || \
        { echo "Error while executing generate_final_model" >> $SDIR/log ; return 1 ; }

    # Prune lexical table
    if [ ${npr_val} -eq 0 -a ${cpr_val} = "0" ]; then
        cp ${curr_tables_dir}/merged_lex_counts ${output}.${lex_ext} || \
            { echo "Error while executing generate_final_model" >> $SDIR/log ; return 1 ; }
    else
        # Prune lexical table
        prune_lex_table || \
            { echo "Error while executing generate_final_model" >> $SDIR/log ; return 1 ; }
    fi
    
    # Copy alignment table if exists
    if [ ${alig_ext} != "none" ]; then
        cp ${curr_tables_dir}/merged_alig_counts ${output}.${alig_ext} || \
            { echo "Error while executing generate_final_model" >> $SDIR/log ; return 1 ; }
    fi

    # Copy log file
    echo "**** Parallel process finished at: "`date` >> $SDIR/log
    cp $SDIR/log ${output}.genswm_log

    # Create sync file
    echo "" > ${sync_info_dir}/generate_final_model
}

remove_temp()
{
    # remove shared directory
    if [ "$debug" -eq 0 ]; then
        rm -rf $SDIR 2>/dev/null
    fi
}

exclude_readonly_vars()
{
    ${AWK} -F "=" 'BEGIN{
                         readonlyvars["BASHOPTS"]=1
                         readonlyvars["BASH_VERSINFO"]=1
                         readonlyvars["EUID"]=1
                         readonlyvars["PPID"]=1
                         readonlyvars["SHELLOPTS"]=1
                         readonlyvars["UID"]=1
                        }
                        {
                         if(!($1 in readonlyvars)) printf"%s\n",$0
                        }'
}

exclude_bashisms()
{
    $AWK '{if(index($1,"=(")==0) printf"%s\n",$0}'
}

write_functions()
{
    for f in `${AWK} '{if(index($1,"()")!=0) printf"%s\n",$1}' $0`; do
        $SED -n /^$f/,/^}/p $0
    done
}

create_script()
{
    # Init variables
    local name=$1
    local command=$2

    # Write environment variables
    set | exclude_readonly_vars | exclude_bashisms > ${name}

    # Write functions if necessary
    $GREP "()" ${name} -A1 | $GREP "{" > /dev/null || write_functions >> ${name}

    # Write PBS directives
    echo "#PBS -o ${name}.o\${PBS_JOBID}" >> ${name}
    echo "#PBS -e ${name}.e\${PBS_JOBID}" >> ${name}
    echo "#$ -cwd" >> ${name}

    # Write command to be executed
    echo "${command}" >> ${name}

    # Give execution permission
    chmod u+x ${name}
}

launch()
{
    local job_deps=$1
    local program=$2
    local suffix=$3
    local outvar=$4

    if [ "${QSUB_WORKS}" = "no" ]; then
        $program &
        eval "${outvar}=$!"
    else
        # Check if the sleep command is used to synchronize processes
        if [ ${sync_sleep} -eq 1 ]; then
            # The sleep command is being used
            # Create script
            create_script ${scripts_dir}/${program}${suffix}.sh $program
            # Execute qsub command
            local jid=$(${QSUB} ${QSUB_TERSE_OPT} ${qs_opts} ${scripts_dir}/${program}${suffix}.sh | ${TAIL} -1)

            # Set value of output variable
            eval "${outvar}='${jid}'"
        else
            # Synchronization is carried out by explicitly defining
            # dependencies between jobs when executing qsub
            
            # Create script
            create_script ${scripts_dir}/${program}${suffix}.sh $program

            # Define qsub option declaring job dependencies
            local depend_opt=""
            if [ ! -z "$job_deps" ]; then
                job_deps=`echo ${job_deps} | $AWK '{for(i=1;i<NF;++i) printf"%s:",$i; printf"%s",$NF}'`
                depend_opt="-W depend=afterok:${job_deps}"
            fi

            # Execute qsub command. The -h option is used to hold
            # jobs. All jobs are released at the end of the script. This
            # ensures that job dependencies are defined over existing
            # jobs
            local jid=$(${QSUB} -h ${depend_opt} ${QSUB_TERSE_OPT} ${qs_opts} ${scripts_dir}/${program}${suffix}.sh | ${TAIL} -1)

            # Set value of output variable
            eval "${outvar}='${jid}'"

            # Uncomment line to show debug information
            # echo $program ${depend_opt} $jid
        fi
    fi
}

job_is_unknown()
{
    nl=`$QSTAT ${QSTAT_J_OPT} ${jid} 2>&1 | grep -e "Unknown" -e "do not exist" | wc -l`
    if [ $nl -ne 0 ]; then
        echo 1
    else
        echo 0
    fi
}

pbs_sync()
{
    # Init vars
    local job_ids=$1
    local pref=$2
    local sync_num_files=`echo "${job_ids}" | $AWK '{printf"%d",NF}'`

    if [ ${sync_sleep} -eq 1 ]; then
        # Execute sync loop
        local sync_end=0
        while [ ${sync_end} -ne 1 ]; do
            sleep 2
            
            # Compare current number of sync files written with the required
            # number
            sync_curr_num_files=`ls -l ${sync_info_dir}/ | grep " ${pref}" | wc -l`
            if [ ${sync_curr_num_files} -eq ${sync_num_files} ]; then
                sync_end=1
            fi
      
            # Sanity check
            # In pbs clusters, check if there are terminated processes
            # that have not written the sync file
            num_running_procs=0
            for jid in ${job_ids}; do
                job_unknown=`job_is_unknown ${jid}`
                if [ ${job_unknown} -eq 0 ]; then
                    num_running_procs=`expr ${num_running_procs} + 1`
                fi
            done
            if [ ${num_running_procs} -eq 0 ]; then
                sync_curr_num_files=`ls -l ${sync_info_dir}/ | grep " ${pref}" | wc -l`
                if [ ${sync_curr_num_files} -ne ${sync_num_files} ]; then
                    echo "Error during synchronization" >&2
                    return 1
                fi
            fi
        done
      
        return 0
    else
        # No sync loop is required
        return 0
    fi
}

all_procs_ok()
{
    # Init variables
    local job_ids=$1
    local pref=$2
    local sync_num_files=`echo "${job_ids}" | $AWK '{printf"%d",NF}'`

    # Obtain number of processes that terminated correctly
    local sync_curr_num_files=`ls -l ${sync_info_dir}/ | $GREP " ${pref}" | wc -l`

    # Return result
    if [ ${sync_num_files} -eq ${sync_curr_num_files} ]; then
        echo "1"
    else
        echo "0"
    fi
}

sync()
{
    # Init vars
    local job_ids=$1
    local pref=$2

    if [ "${QSUB_WORKS}" = "no" ]; then
        wait
        sync_ok=`all_procs_ok "${job_ids}" $pref`
        if [ $sync_ok -eq 1 ]; then
            return 0
        else
            return 1
        fi
    else
        pbs_sync "${job_ids}" $pref
    fi
}

release_job_holds()
{
    job_ids=$1
    ${QRLS} ${job_ids}
    
    # Uncomment line to get debugging information
    # echo ${job_id_list}
}

print_iter_num_message()
{
    echo "*** EM iteration ${n} out of $niters (started at "`date`")..." >> $SDIR/log

    # Create sync file
    echo "" > ${sync_info_dir}/print_iter_num_message
}

print_merge_start_message()
{
    echo "** Merging counts (started at "`date`")..." >> $SDIR/log

    # Create sync file
    echo "" > ${sync_info_dir}/print_merge_start_message
}

print_create_filt_models_message()
{
    echo "** Creating filtered models (started at "`date`")..." >> $SDIR/log

    # Create sync file
    echo "" > ${sync_info_dir}/print_create_filt_models_message
}

gen_log_err_files()
{
    if [ ${sync_sleep} -eq 1 ]; then
        if [ -f $SDIR/log ]; then
            cp $SDIR/log ${output}.genswm_log
        fi

        # Generate file for error diagnosing
        if [ -f ${slmodel_dir}/log ]; then
            cat ${slmodel_dir}/log > ${output}.genswm_err
        fi
        # Gather info about each iteration
        nit=1
        while [ $nit -le ${niters} ]; do
            echo "*** EM iteration ${nit} out of $niters" >> ${output}.genswm_err
            for f in ${models_per_chunk_dir}/*_proc_n${nit}.log; do
                if [ -f $f ]; then
                    cat $f >> ${output}.genswm_err
                fi
            done

            if [ -f ${curr_tables_dir}/merge_lex_n${nit}.log ]; then 
                cat ${curr_tables_dir}/merge_lex_n${nit}.log >> ${output}.genswm_err
            fi

            if [ -f ${curr_tables_dir}/merge_alig_n${nit}.log ]; then 
                cat ${curr_tables_dir}/merge_alig_n${nit}.log >> ${output}.genswm_err
            fi

            for f in ${filtered_model_dir}/*_filt_n${nit}.log; do
                cat $f >> ${output}.genswm_err 2> /dev/null
            done

            nit=`expr $nit + 1`
        done

        if [ -f ${curr_tables_dir}/generate_final_model.log ]; then
            cat ${curr_tables_dir}/generate_final_model.log >> ${output}.genswm_err
        fi
    fi
}

report_errors()
{
    if [ ${sync_sleep} -eq 1 ]; then
        num_err=`$GREP "Error while executing" ${output}.genswm_log | wc -l`
        if [ ${num_err} -gt 0 ]; then
            # Print error messages
            prog=`$GREP "Error while executing" ${output}.genswm_log | head -1 | $AWK '{printf"%s",$4}'`
            echo "Error during the execution of thot_pbs_gen_batch_sw_model (${prog})" >&2
            if [ -f ${output}.genswm_err ]; then
                echo "File ${output}.genswm_err contains information for error diagnosing" >&2
            fi
         else
            echo "Synchronization error" >&2
            if [ -f ${output}.genswm_err ]; then
                echo "File ${output}.genswm_err contains information for error diagnosing" >&2
            fi
        fi
    fi
}

# main
pr_given=0
sdir=$HOME
s_given=0
t_given=0
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
shu_given=0
qs_given=0
tdir="/tmp"
sync_sleep=1
debug=0
local_ch_size=250000

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
        "-pr") shift
            if [ $# -ne 0 ]; then
                pr_val=$1
                pr_given=1
            fi
            ;;
        "-sdir") shift
            if [ $# -ne 0 ]; then
                sdir=$1                
            fi
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
        "-qs") shift
            if [ $# -ne 0 ]; then
                qs_opts=$1
                qs_given=1
            else
                qs_given=0
            fi
            ;;
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1
            else
                tdir="./"
            fi
            ;;
        "-lc") shift
            if [ $# -ne 0 ]; then
                local_ch_size=$1
            fi
            ;;
        "-shu") shu_given=1
            ;;
        "-debug") debug=1
            ;;
        "--sync-dep") sync_sleep=0
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

if [ ${pr_given} -eq 0 ]; then
    # invalid parameters 
    echo "Error: number of processors must be given" >&2
    exit 1
fi

# parameters are ok

# Set TMP directory
set_tmp_dir || exit 1

# Set shared directory (global variables are declared)
declare chunks_dir="" 
declare init_model_dir="" 
declare curr_tables_dir="" 
declare models_per_chunk_dir=""
declare filtered_model_dir="" 
declare slmodel_dir=""
declare scripts_dir="" 
declare used_scripts_dir=""
declare sync_info_dir=""

set_shared_dir || exit 1

# Output info about tracking script progress
echo "NOTE: see file ${SDIR}/log to track model estimation progress" >&2

# Create log file
echo "**** Parallel process started at: "`date` > $SDIR/log

# Determine file format
declare file_format=""
determine_file_format

# Estimate initial model with complete vocabulary
estimate_init_model || exit 1

# Get model information (extracted from the initial model files)
declare lex_ext=""
declare alig_ext=""
get_model_information

# Split shuffled input into chunks and process them separately...
# job_deps=""
# launch "${job_deps}" split_input "" spl_job_id || exit 1
# sync "${spl_job_id}" "split_input" || { gen_log_err_files ; report_errors ; exit 1; }
split_input
spl_job_id=""

# Estimate sentence length model
job_deps=${spl_job_id}
launch "${job_deps}" estimate_slmodel "" slm_job_id || exit 1
sync "${slm_job_id}" "estimate_slmodel" || { gen_log_err_files ; report_errors ; exit 1; }

# Declare job id list variable
declare job_id_list=""

# Update job_id_list
job_id_list="${spl_job_id} ${slm_job_id}"

# EM algorithm iterations
n=1
while [ $n -le ${niters} ]; do
    # Print message with EM iteration number
    if [ $n -eq 1 ]; then
        job_deps=${slm_job_id}
    else
        job_deps=${f_job_ids}
    fi
    launch "${job_deps}" print_iter_num_message "_n${n}" print_iter_job_id || exit 1
    # Print synchronization
    sync "${print_iter_job_id}" "print_iter_num_message" || { gen_log_err_files ; report_errors ; exit 1; }
 
    # Init variables
    chunk_id=0
    pc_job_ids=""

    # Train models for chunks
    for i in `ls ${chunks_dir}/src\_chunk\_*`; do
        # Initialize variables
        chunk=`${BASENAME} $i`
        chunk=${chunk:4}
        src_chunk="src_"${chunk}
        trg_chunk="trg_"${chunk}
        out_chunk="out_"${chunk}
        chunk_id=`expr $chunk_id + 1`
        
        # Process chunk
        job_deps=${print_iter_job_id}
        launch "${job_deps}" proc_chunk "_${chunk_id}_n${n}" job_id || exit 1
        pc_job_ids=${job_id}" "${pc_job_ids}
    done

    # Training synchronization
    sync "${pc_job_ids}" "proc_chunk" || { gen_log_err_files ; report_errors ; exit 1; }

    # Merge counts for submodels

    # Print message reporting the start of merging process
    job_deps=${pc_job_ids}
    launch "${job_deps}" print_merge_start_message "_n${n}" print_merge_job_id || exit 1
    # Print synchronization
    sync "${print_merge_job_id}" "print_merge_start_message" || { gen_log_err_files ; report_errors ; exit 1; }

    # Check whether to execute merging of lex and alig counts
    # concurrently
    if [ ${pr_val} -ge 2 ]; then
        # Merge lexical counts
        job_deps=${print_merge_job_id}
        launch "${job_deps}" merge_lex_counts "_n${n}" lex_job_id || exit 1

        # Merge alignment counts
        job_deps=${print_merge_job_id}
        launch "${job_deps}" merge_alig_counts "_n${n}" alig_job_id || exit 1

        merge_job_ids="${lex_job_id} ${alig_job_id}"

        # Merge synchronization
        sync "${merge_job_ids}" "merge" || { gen_log_err_files ; report_errors ; exit 1; }
    else
        job_deps=${print_merge_job_id}
        launch "${job_deps}" merge_lex_counts "_n${n}" lex_job_id || exit 1
        sync "${lex_job_id}" "merge_lex_counts" || { gen_log_err_files ; report_errors ; exit 1; }

        job_deps=${lex_job_id}
        launch "${job_deps}" merge_alig_counts "_n${n}" alig_job_id || exit 1
        sync "${alig_job_id}" "merge_alig_counts" || { gen_log_err_files ; report_errors ; exit 1; }

        merge_job_ids="${lex_job_id} ${alig_job_id}"
    fi

    # Create filtered models if not in last iteration
    if [ $n -lt ${niters} ]; then

        # Print message reporting the start of the filtering process
        job_deps=${merge_job_ids}
        launch "${job_deps}" print_create_filt_models_message "_n${n}" print_create_filt_job_id || exit 1
        # Print synchronization
        sync "${print_create_filt_job_id}" "print_create_filt_models_message" || { gen_log_err_files ; report_errors ; exit 1; }
            
        f_job_ids=""
        for i in `ls ${chunks_dir}/src\_chunk\_*`; do
            # Initialize variables
            chunk=`${BASENAME} $i`
            chunk=${chunk:4}
            src_chunk="src_"${chunk}
            trg_chunk="trg_"${chunk}
            out_chunk="out_"${chunk}
            chunk_id=`expr $chunk_id + 1`
            
            # Create filtered model for chunk
            job_deps=${print_create_filt_job_id}
            launch "${job_deps}" create_filtered_model "_n${n}" job_id || exit 1
            f_job_ids=${job_id}" "${f_job_ids}
        done
        # Filter synchronization
        sync "${f_job_ids}" "create_filtered_model" || { gen_log_err_files ; report_errors ; exit 1; }
    fi

    # Move scripts to the used scripts directory (required when using
    # sleep-based synchronization)
    if [ ${sync_sleep} -eq 1 ]; then
        mv ${scripts_dir}/* ${used_scripts_dir}/ 2> /dev/null
    fi

    # Remove sync info if sleep-based synchronization is being performed
    if [ ${sync_sleep} -eq 1 ]; then
        rm -rf ${sync_info_dir}/*
    fi

    # Update job_id_list
    job_id_list="${print_iter_job_id} ${pc_job_ids} ${print_merge_job_id} ${merge_job_ids} ${print_create_filt_job_id} ${f_job_ids} ${job_id_list}"

    # Increase n (num iter)
    n=`expr $n + 1`
done

# Generate final model
if [ $n -lt ${niters} ]; then
    job_deps=${f_job_ids}
else
    job_deps=${merge_job_ids}
fi
launch "${job_deps}" generate_final_model "" gfm_job_id || exit 1

# Update job_id_list
job_id_list="${gfm_job_id} ${job_id_list}"

# Generate final model synchronization
sync ${gfm_job_id} "generate_final_model" || { gen_log_err_files ; report_errors ; exit 1; }

# Generate log and err files
gen_log_err_files

# Remove temporary files
if [ ${sync_sleep} -eq 1 ]; then
    # Sync using sleep is enabled
    mv ${scripts_dir}/* ${used_scripts_dir}/ 2> /dev/null
    remove_temp
else
    # Sync using sleep is not enabled
    job_deps=${gfm_job_id}
    launch "${job_deps}" remove_temp "" rt_job_id || exit 1
    # Update job_id_list
    job_id_list="${rt_job_id} ${job_id_list}"
    # Remove temporary files synchronization
    sync ${rt_job_id} "remove_temp" || { report_errors ; exit 1; }
fi

# Release job holds
if [ ! "${QSUB_WORKS}" = "no" -a ${sync_sleep} -eq 0 ]; then
    release_job_holds "${job_id_list}"
fi
