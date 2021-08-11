# Author: Daniel Ortiz Mart\'inez
# *- bash -*

#####################
# GENERAL FUNCTIONS #
#####################

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

esc_dq()
{
    "$SED" 's/"/\\\"/g' <<< "$1"
}

deesc_dq()
{
    "$SED" 's/\\\"/"/g' <<< "$1"
}

get_lm_from_cfgfile()
{
    local cmdline_cfg=$1
    local lm
    lm="`"$GREP" "\-lm " "$cmdline_cfg" | "$AWK" '{printf"%s",$2}'`"
    echo "$lm"
}

get_tm_from_cfgfile()
{
    local cmdline_cfg=$1
    local tm
    tm="`"$GREP" "\-tm " "$cmdline_cfg" | "$AWK" '{printf"%s",$2}'`"
    echo "$tm"
}
