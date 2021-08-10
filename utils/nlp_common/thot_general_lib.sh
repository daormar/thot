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
