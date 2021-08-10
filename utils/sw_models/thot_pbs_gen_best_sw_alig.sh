# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Generates the best single word alignment for the sentence pairs contained in a parallel corpus.

# INCLUDE BASH LIBRARIES
. "${bindir}"/thot_general_lib || exit 1
. "${bindir}"/thot_adv_sched_lib || exit 1

print_desc()
{
    echo "thot_pbs_gen_best_sw_alig written by Daniel Ortiz"
    echo "thot_pbs_gen_best_sw_alig generates the best word alignment for a parallel corpus"
    echo "type \"thot_pbs_gen_best_sw_alig --help\" to get usage information"
}

version()
{
    echo "thot_pbs_gen_best_sw_alig is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

usage()
{
    echo "thot_pbs_gen_best_sw_alig -pr <int> -sw <string>"
    echo "                      -s <string> -t <string> -o <string>"
    echo "                      [-shu] [-qs <string>]"
    echo "                      [-tdir <string>] [-sdir <string>]"
    echo "                      [--sync-dep] [-debug] [--help] [--version]"
    echo ""
    echo "-pr <int>          : Number of processors."
    echo "-sw <string>       : Prefix of single word model files."
    echo "                     NOTES:"
    echo "                      a) give absolute paths when using pbs clusters."
    echo "                      b) ensure that the given path is reachable by all nodes."
    echo "-s <string>        : File with source sentences (give absolute path when"
    echo "                     using pbs clusters)."
    echo "-t <string>        : File with target sentences (give absolute path when"
    echo "                     using pbs clusters)."
    echo "-o <string>        : Output prefix (give absolute path when"
    echo "                     using pbs clusters)."
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

model_access_is_process_safe()
{
    nlines=`"${bindir}"/thot_server -i 2>&1 | "$GREP" 'model reads are not process-safe for swm module' | "$WC" -l | "$AWK" '{print $1}'`
    if [ $nlines -eq 0 ]; then
        echo 'yes'
    else
        echo 'no'
    fi
}

set_tmp_dir()
{
    if [ -d "${tdir}" ]; then
        TMP=${tdir}
    else
        echo "Error: temporary directory does not exist" >&2
        return 1;
    fi
}

set_shared_dir()
{
    if [ ! -d "${sdir}" ]; then
        echo "Error: shared directory does not exist" >&2
        return 1;
    fi

    SDIR="${sdir}/thot_pbs_gen_best_sw_alig_sdir_${PPID}_$$"
    mkdir "$SDIR" || { echo "Error: shared directory cannot be created" >&2 ; return 1; }

    # Create temporary subdirectories
    chunks_dir="$SDIR"/chunks
    scripts_dir="$SDIR"/scripts
    aligs_per_chunk_dir="$SDIR"/aligs_per_chunk
    sync_info_dir="$SDIR"/sync
    mkdir "${chunks_dir}" || return 1
    mkdir "${scripts_dir}" || return 1
    mkdir "${aligs_per_chunk_dir}" || return 1
    mkdir "${sync_info_dir}" || return 1

    # Function executed correctly
    return 0
}

split_input()
{
    echo "*** Shuffling and splitting input: ${srcf} ${trgf}..." >> $SDIR/log

    # Determine fragment size
    local input_size=`wc -l "${srcf}" 2>/dev/null | "${AWK}" '{printf"%d",$1}'`
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
        "${SPLIT}" -l ${chunk_size} "${srcf}" "${chunks_dir}/src_chunk_" || return 1
        "${SPLIT}" -l ${chunk_size} "${trgf}" "${chunks_dir}/trg_chunk_" || return 1
    else
        local rand_seed=31415
        "${bindir}"/thot_shuffle ${rand_seed} "${tdir}" "${srcf}" | "${SPLIT}" -l ${chunk_size} - "${chunks_dir}/src_chunk_" || return 1
        "${bindir}"/thot_shuffle ${rand_seed} "${tdir}" "${trgf}" | "${SPLIT}" -l ${chunk_size} - "${chunks_dir}/trg_chunk_" || return 1
    fi
}

remove_temp()
{
    # remove shared directory
    if [ "$debug" -eq 0 ]; then
        rm -rf "$SDIR" 2>/dev/null
    fi
}

proc_chunk()
{
    # Write date to log file
    echo "** Processing chunk ${chunk} (started at "`date`")..." >> "$SDIR"/log
    echo "** Processing chunk ${chunk} (started at "`date`")..." > "${aligs_per_chunk_dir}"/${chunk}_bestal.log

    "${bindir}"/thot_format_corpus_csl "${chunks_dir}"/${src_chunk} "${chunks_dir}"/${trg_chunk} \
        2>> "${aligs_per_chunk_dir}"/${chunk}_bestal.log | \
        "${bindir}"/thot_calc_swm_lgprob -sw ${sw_val} -P - -max \
        2>> "${aligs_per_chunk_dir}"/${chunk}_bestal.log > "${aligs_per_chunk_dir}"/${chunk}_bestal ; ${PIPE_FAIL} || \
        { echo "Error while executing proc_chunk for ${chunk}" >> "$SDIR"/log ; return 1; }

    # Write date to log file
    echo "Processing of chunk ${chunk} finished ("`date`")" >> "$SDIR"/log 

    # Create sync file
    echo "" > "${sync_info_dir}"/proc_chunk_${chunk}

    return 0
}

generate_alig_file()
{
    # Write date to log file
    echo "** Generate alignment file (started at "`date`")..." >> "$SDIR"/log
    echo "** Generate alignment file (started at "`date`")..." > "${aligs_per_chunk_dir}"/merge.log

    # Delete output file if exists
    if [ -f "${output}".bestal ]; then
        rm "${output}".bestal
    fi

    # Dump alignments in output file
    for f in "${aligs_per_chunk_dir}"/*_bestal; do
        cat "$f" 2>> "${aligs_per_chunk_dir}"/merge.log >> "${output}".bestal || \
            { echo "Error while executing generate_alig_file" >> "$SDIR"/log ; return 1; }
    done

    # Copy log file
    echo "**** Parallel process finished at: "`date` >> "$SDIR"/log
    cp "$SDIR"/log "${output}".genb_log

    # Create sync file
    echo "" > "${sync_info_dir}"/generate_alig_file
}

gen_log_err_files()
{
    if [ ${sync_sleep} -eq 1 ]; then
        if [ -f $SDIR/log ]; then
            cp "$SDIR"/log "${output}".genb_log
        fi
        
        # Generate file for error diagnosing
        if [ -f "${output}".genb_err ]; then
            rm "${output}".genb_err
        fi

        for f in "${aligs_per_chunk_dir}"/*_bestal.log; do
            cat "$f" > "${output}".genb_err
        done

        if [ -f "${aligs_per_chunk_dir}"/merge.log ]; then
            cat "${aligs_per_chunk_dir}"/merge.log >> "${output}".genb_err
        fi
    fi
}

report_errors()
{
    if [ ${sync_sleep} -eq 1 ]; then
        num_err=`"$GREP" "Error while executing" "${output}".genb_log | wc -l`
        if [ ${num_err} -gt 0 ]; then
            # Print error messages
            prog=`"$GREP" "Error while executing" ${output}.genb_log | head -1 | "$AWK" '{printf"%s",$4}'`
            echo "Error during the execution of thot_pbs_gen_best_sw_alig (${prog})" >&2
            if [ -f "${output}".genb_err ]; then
                echo "File ${output}.genb_err contains information for error diagnosing" >&2
            fi
         else
            echo "Synchronization error" >&2
            if [ -f "${output}".genb_err ]; then
                echo "File ${output}.genb_err contains information for error diagnosing" >&2
            fi
       fi
    fi
}

# main
pr_given=0
sw_given=0
shu_given=0
qs_given=0
sdir=$HOME
s_given=0
t_given=0
o_given=0
tdir="/tmp"
sync_sleep=1
debug=0

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
        "-sw") shift
            if [ $# -ne 0 ]; then
                sw_val=$1
                sw_given=1
            else
                sw_given=0
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

if [ ${sw_given} -eq 0 ];then
    # invalid parameters 
    echo "Error: prefix of single word model files must be given" >&2
    exit 1
fi

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

if [ ${pr_given} -eq 0 ]; then
    # invalid parameters 
    echo "Error: number of processors must be given" >&2
    exit 1
fi

process_safety=`model_access_is_process_safe`
if [ ${process_safety} = "no" ]; then
    if [ ${pr_val} -gt 1 ]; then
        echo "Warning: only one processor will be used since single word model module is not process-safe" >&2
        pr_val=1
    fi
fi

# parameters are ok

# Set TMP directory
set_tmp_dir || exit 1

# Set shared directory (global variables are declared)
declare chunks_dir="" 
declare aligs_per_chunk_dir=""
declare scripts_dir=""
declare sync_info_dir=""

set_shared_dir || exit 1

# Output info about tracking script progress
echo "NOTE: see file ${SDIR}/log to track best alignment generation progress" >&2

# Create log file
echo "**** Parallel process started at: "`date` > "$SDIR"/log

# Split shuffled input into chunks and process them separately...
# job_deps=""
split_input || exit 1

# Declare job id list variable
declare job_id_list=""

# Init variables
chunk_id=0

# Generate best alignment
for i in "${chunks_dir}/src_chunk_"*; do
    # Initialize variables
    chunk=`"${BASENAME}" $i`
    chunk=${chunk:4}
    src_chunk="src_${chunk}"
    trg_chunk="trg_${chunk}"
    chunk_id=`expr $chunk_id + 1`
        
    # Process chunk
    job_deps=""
    launch "${job_deps}" "${scriptsdir}" proc_chunk "_${chunk_id}" job_id || exit 1
    pc_job_ids=${job_id}" "${pc_job_ids}
done

# Alignment generation synchronization
sync "${pc_job_ids}" "proc_chunk" || { gen_log_err_files ; report_errors ; exit 1; }

# Generate alignment file
job_deps=${pc_job_ids}
launch "${job_deps}" "${scriptsdir}" generate_alig_file "" gaf_job_id || exit 1

# Generate alignment file synchronization
sync ${gaf_job_id} "generate_alig_file" || { gen_log_err_files ; report_errors ; exit 1; }

# Generate log and err files
gen_log_err_files

# Remove temporary files
if [ ${sync_sleep} -eq 1 ]; then
    # Sync using sleep is enabled
    remove_temp
else
    # Sync using sleep is not enabled
    job_deps=${gfm_job_id}
    launch "${job_deps}" "${scriptsdir}" remove_temp "" rt_job_id || exit 1
    # Update job_id_list
    job_id_list="${rt_job_id} ${job_id_list}"
    # Remove temporary files synchronization
    sync ${rt_job_id} "remove_temp" || { report_errors ; exit 1; }
fi

# Update job_id_list
job_id_list="${pc_job_ids} ${gaf_job_id} ${rt_job_id}"

# Release job holds
if [ ! "${QSUB_WORKS}" = "no" -a ${sync_sleep} -eq 0 ]; then
    release_job_holds "${job_id_list}"
fi
