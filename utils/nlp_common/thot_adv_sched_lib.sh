# Author: Daniel Ortiz Mart\'inez
# *- bash -*

#####################
# GENERAL FUNCTIONS #
#####################

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
    set | exclude_readonly_vars | exclude_bashisms > ${name}

    # Write functions if necessary
    "$GREP" "()" "${name}" -A1 | "$GREP" "{" > /dev/null || write_functions >> ${name}

    # Write PBS directives
    stream_fname=`${BASENAME} ${name}`
    echo "#PBS -o ${stream_fname}.o\${PBS_JOBID}" >> "${name}"
    echo "#PBS -e ${stream_fname}.e\${PBS_JOBID}" >> "${name}"
    echo "#$ -cwd" >> ${name}

    # Write command to be executed
    echo "${command}" >> "${name}"

    # Give execution permission
    chmod u+x "${name}"
}

launch()
{
    local job_deps=$1
    local scriptsdir=$2
    local program=$3
    local suffix=$4
    local outvar=$5

    if [ "${QSUB_WORKS}" = "no" ]; then
        "$program" &
        eval "${outvar}=$!"
    else
        # Check if the sleep command is used to synchronize processes
        if [ ${sync_sleep} -eq 1 ]; then
            # The sleep command is being used
            # Create script
            create_script "${scripts_dir}/${program}${suffix}.sh" "$program"
            # Execute qsub command
            local jid=$("${QSUB}" ${QSUB_TERSE_OPT} ${qs_opts} "${scripts_dir}/${program}${suffix}.sh" | "${TAIL}" -1)

            # Set value of output variable
            eval "${outvar}='${jid}'"
        else
            # Synchronization is carried out by explicitly defining
            # dependencies between jobs when executing qsub
            
            # Create script
            create_script "${scripts_dir}/${program}${suffix}.sh" $program

            # Define qsub option declaring job dependencies
            local depend_opt=""
            if [ ! -z "$job_deps" ]; then
                job_deps=`echo ${job_deps} | "$AWK" '{for(i=1;i<NF;++i) printf"%s:",$i; printf"%s",$NF}'`
                depend_opt="-W depend=afterok:${job_deps}"
            fi
            
            # Execute qsub command. The -h option is used to hold
            # jobs. All jobs are released at the end of the script. This
            # ensures that job dependencies are defined over existing
            # jobs
            local jid=$("${QSUB}" ${QSUB_TERSE_OPT} -h ${depend_opt} ${qs_opts} "${scripts_dir}/${program}${suffix}.sh" | ${TAIL} -1)

            # Set value of output variable
            eval "${outvar}='${jid}'"
        fi
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
    # Init vars
    local job_ids=$1
    local pref=$2
    local sync_num_files=`echo "${job_ids}" | "$AWK" '{printf"%d",NF}'`

    if [ ${sync_sleep} -eq 1 ]; then
        # Execute sync loop
        local sync_end=0
        while [ ${sync_end} -ne 1 ]; do
            sleep 2
            
            # Compare current number of sync files written with the required
            # number
            sync_curr_num_files=`ls -l "${sync_info_dir}/" | "$GREP" " ${pref}" | wc -l`
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
                sync_curr_num_files=`ls -l "${sync_info_dir}/" | "$GREP" " ${pref}" | wc -l`
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
    local sync_num_files=`echo "${job_ids}" | "$AWK" '{printf"%d",NF}'`

    # Obtain number of processes that terminated correctly
    local sync_curr_num_files=`ls -l "${sync_info_dir}/" | "$GREP" " ${pref}" | wc -l`

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
        sync_ok=`all_procs_ok "${job_ids}" "$pref"`
        if [ $sync_ok -eq 1 ]; then
            return 0
        else
            return 1
        fi
    else
        pbs_sync "${job_ids}" "$pref"
    fi
}

release_job_holds()
{
    job_ids=$1
    ${QRLS} ${job_ids}    
}
