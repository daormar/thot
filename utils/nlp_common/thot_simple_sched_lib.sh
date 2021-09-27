# Author: Daniel Ortiz Mart\'inez
# *- bash -*

#############
# CONSTANTS #
#############

ARG_SEP="<_ARG_SEP_>"

#####################
# GENERAL FUNCTIONS #
#####################

esc_dq()
{
    local escaped_str=${1//\"/\\\"};
    echo "${escaped_str}"
}

exclude_readonly_vars()
{
    "${AWK}" -F "=" 'BEGIN{
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
    "$AWK" '{if(index($1,"=(")==0) printf"%s\n",$0}'
}

write_functions()
{
    for f in `"${AWK}" '{if(index($1,"()")!=0) printf"%s\n",$1}' $0`; do
        "$SED" -n /^$f/,/^}/p $0
    done
}

create_script()
{
    # Init variables
    local name=$1
    local command=$2

    # Write environment variables
    set | exclude_readonly_vars > "${name}"

    # Write functions if necessary
    "$GREP" "()" "${name}" -A1 | "$GREP" "{" > /dev/null || write_functions >> "${name}"

    # Write PBS directives
    stream_fname=`"${BASENAME}" ${name}`
    echo "#PBS -o \"$(esc_dq "${stream_fname}")\".o\${PBS_JOBID}" >> "${name}"
    echo "#PBS -e \"$(esc_dq "${stream_fname}")\".e\${PBS_JOBID}" >> "${name}"
    echo "#$ -cwd" >> "${name}"

    # Write command to be executed
    echo "${command}" >> "${name}"

    # Give execution permission
    chmod u+x "${name}"
}

launch()
{
    local file=$1
    local outvar=$2

    ### qsub invocation
    if [ "${QSUB_WORKS}" = "no" ]; then
        "$file" &
        eval "${outvar}=$!"
    else
        local jid=$("$QSUB" ${QSUB_TERSE_OPT} ${qs_opts} "$file" | "${TAIL}" -1)
        eval "${outvar}='${jid}'"
    fi
    ###################
}

all_procs_ok()
{
    # Init variables
    local files="$1"

    # Convert string to array
    local preproc_files
    preproc_files=`echo "${files}" | ${SED} "s/${ARG_SEP}/\n/g"`
    local file_array=()
    while IFS= read -r; do file_array+=( "${REPLY}" ); done <<< "${preproc_files}"    

    # Obtain number of processes that terminated correctly
    local sync_num_files=${#file_array[@]}
    local sync_curr_num_files=0
    for f in "${file_array[@]}"; do
        if [ -f "${f}_end" ]; then
            sync_curr_num_files=`expr ${sync_curr_num_files} + 1`
        fi
    done

    # Return result
    if [ ${sync_num_files} -eq ${sync_curr_num_files} ]; then
        echo "1"
    else
        echo "0"
    fi
}

add_sync_file()
{
    local prev_files=$1
    local new_file=$2

    if [ -z "${prev_files}" ]; then
        echo "${new_file}"
    else
        echo "${prev_files}${ARG_SEP}${new_file}"
    fi
}

sync()
{
    # Init vars
    local files="$1"
    local job_ids="$2"

    if [ "${QSUB_WORKS}" = "no" ]; then
        wait
        sync_ok=`all_procs_ok "$files"`
        if [ $sync_ok -eq 1 ]; then
            return 0
        else
            return 1
        fi
    else
        pbs_sync "$files" "${job_ids}"
    fi
}

job_is_unknown()
{
    nl=`"$QSTAT" ${QSTAT_J_OPT} ${jid} 2>&1 | "$GREP" -e "Unknown" -e "do not exist" | wc -l`
    if [ $nl -ne 0 ]; then
        echo 1
    else
        echo 0
    fi
}

pbs_sync()
{
    local files="$1"
    local job_ids="$2"
    local num_pending_procs=0

    # Convert string to array
    local preproc_files
    preproc_files=`echo "${files}" | ${SED} "s/${ARG_SEP}/\n/g"`
    local file_array=()
    while IFS= read -r; do file_array+=( "${REPLY}" ); done <<< "${preproc_files}"
    
    end=0
    while [ $end -ne 1 ]; do
        sleep 3
        end=1
        # Check if all processes have finished
        for f in "${file_array[@]}"; do
            if [ ! -f "${f}_end" ]; then
                num_pending_procs=`expr ${num_pending_procs} + 1`
                end=0
            fi
        done

        # Sanity check
        num_running_procs=0
        for jid in ${job_ids}; do
            job_unknown=`job_is_unknown ${jid}`
            if [ ${job_unknown} -eq 0 ]; then
                num_running_procs=`expr ${num_running_procs} + 1`
            fi
        done
        if [ ${num_running_procs} -eq 0 -a ${num_pending_procs} -ge 1 ]; then
            return 1
        fi
    done
}
