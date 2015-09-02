# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# minimize_trg_func

print_desc()
{
    echo "thot_dhs_min written by Daniel Ortiz."
    echo "thot_dhs_min allows to minimize a given target function." 
    echo "type \"thot_dhs_min --help\" to get usage information."
}

version()
{
    echo "thot_dhs_min is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

usage()
{
    echo "thot_dhs_min       [-tdir <string>] -va <float> ... <float>"
    echo "                   [-iv <float> ... <float>]"
    if [ "${USE_NR_ROUTINES}" = "yes" ]; then
    echo "                   [-l <float>] [-b <float>]"
    fi
    echo "                   -u <string> -o <string> [-ftol <float>]"
    echo "                   [-r <string>] [-debug] [-v] [--help] [--version]"
    echo " -tdir <string>       : Absolute path of a directory for storing temporary"
    echo "                        files. If not given /tmp is used."
    echo " -va <float>...<float>: Set fixed and non-fixed variable values."
    echo "                        The number of variables and their meaning depends"
    echo "                        on the target function you want to adjust."
    echo "                        Each value equal to -0 is considered a non fixed value."
    echo " -iv <float>...<float>: Initial values for the variables (fixed values set by"
    echo "                        -va are not affected by -iv)."
    if [ "${USE_NR_ROUTINES}" = "yes" ]; then
    echo " -l <float>           : Value of lambda used to generate the initial simplex."
    echo " -b <float>           : Bias value used to generate the initial simplex."
    fi
    echo " -u <string>          : Path of the executable file involved in the calculation"
    echo "                        of the target function to minimize given the values of"
    echo "                        a set of variables."
    echo " -o <string>          : Set output files prefix name."
    echo " -ftol <float>        : Fractional convergence tolerance"
    echo "                        (${ftol} by default)."
    echo " -r <string>          : Resume weight adjustment using file <string> containing"
    echo "                        a previously generated adj.img file."
    echo " -v                   : Verbose mode."
    echo " -debug               : After ending, do not delete temporary files"
    echo "                        (for debugging purposes)."
    echo " --help               : Display this help and exit."
    echo " --version            : Output version information and exit."
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

str_is_option()
{
    echo "" | ${AWK} -v s=$1 '{if(!match(s,"-[a-zA-Z]")) print "0"; else print "1"}' 
}

tdir=""
va_given=0
iv_opt=""
lambda_opt=""
bias_opt=""
u_given=0
o_given=0

if [ "${USE_NR_ROUTINES}" = "yes" ]; then
    ftol=0.01
else
    ftol=0.0001
fi

r_given=0
verbose_opt=""
debug=""

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
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1                
            else
                tdir=""
            fi
            ;;
        "-va") shift
            if [ $# -ne 0 -a ! "`str_is_option $1`" = "1" ]; then
                end=0
                while [ $end -eq 0 ]; do
                    vars=$vars" "$1
                    if [ $# -le 1 ]; then
                        end=1
                    else
                        if [ "`str_is_option $2`" = "1" ]; then
                            end=1
                        else
                            shift
                        fi
                    fi
                done            
                va_given=1
            fi
            ;;
        "-iv") shift
            if [ $# -ne 0 -a ! "`str_is_option $1`" = "1" ]; then
                end=0
                while [ $end -eq 0 ]; do
                    init_vals=$init_vals" "$1
                    if [ $# -le 1 ]; then
                        end=1
                    else
                        if [ "`str_is_option $2`" = "1" ]; then
                            end=1
                        else
                            shift
                        fi
                    fi
                done            
                iv_opt="-iv ${init_vals}"
            fi
            ;;
        "-l") shift
            if [ $# -ne 0 ]; then
                lambda_opt="-l $1"
            fi
            ;;
        "-b") shift
            if [ $# -ne 0 ]; then
                bias_opt="-b $1"
            fi
            ;;
        "-u") shift
            if [ $# -ne 0 ]; then
                target_func=$1
                u_given=1
            else
                u_given=0
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                outpref=$1
                o_given=1
            else
                o_given=0
            fi
            ;;
        "-ftol") shift
            if [ $# -ne 0 ]; then
                ftol=$1
            fi
            ;;
        "-r") shift
            if [ $# -ne 0 ]; then
                r_val=$1
                r_given=1
            else
                r_given=0
            fi
            ;;
        "-debug") debug="-debug"
            ;;
        "-v") verbose_opt="-v"
            ;;
    esac
    shift
done

# verify parameters

if [ ${va_given} -eq 0 ]; then
    # invalid parameters 
    echo "Error: -va option not given" >&2 
    exit 1
fi

if [ ${o_given} -eq 0 ]; then
    # invalid parameters 
    echo "Error: -o option not given" >&2 
    exit 1
fi

if [ ${u_given} -eq 0 ]; then
    # invalid parameters 
    echo "Error: -u option not given" >&2 
    exit 1
else
    if [ ! -x ${target_func} ]; then
        echo "Error: ${target_func} cannot be executed" >&2 
        exit 1
    fi
fi

if [ ${r_given} -eq 1 ]; then
    if [ ! -f ${r_val} ]; then
        # invalid parameters 
        echo "Error: file ${r_val} to resume weight adjustment does not exist" >&2 
        exit 1
    fi
fi

# parameters are ok

# create shared directory
if [ -z "$tdir" ]; then
    # if not given, TDIR_DHS will be the /tmp directory
    TDIR_DHS="/tmp/thot_dhs_min_$$"
    mkdir $TDIR_DHS || { echo "Error: shared directory cannot be created" >&2 ; exit 1; }

else
    TDIR_DHS="${tdir}/thot_dhs_min_$$"
    mkdir $TDIR_DHS || { echo "Error: shared directory cannot be created" >&2 ; exit 1; }
fi

# remove temp directories on exit
if [ "$debug" != "-debug" ]; then
    trap "rm -rf $TDIR_DHS 2>/dev/null" EXIT
fi

# Execute loop until step by step downhill simplex returns the final
# values for the variables
end=0

# Set initial file with objective function images
if [ ${r_given} -eq 1 ]; then
    cp ${r_val} ${TDIR_DHS}/adj.img
    cp ${r_val} ${TDIR_DHS}/adj.img.old
    cat ${r_val} | $AWK '{printf"<resumed xval>\n"}' > ${TDIR_DHS}/adj.xval
else
    echo "" | $AWK '{printf""}' > ${TDIR_DHS}/adj.img
fi

# Downhill simplex algorithm loop
echo "Starting downhill simplex optimization..." >&2
echo "NOTE: see last lines of file $TDIR_DHS/adj.log to track optimization progress" >&2
while [ $end -ne 1 ]; do
    if [ "${USE_NR_ROUTINES}" = "yes" ]; then
        ${bindir}/dhs_step_by_step_min_nr -va ${vars} ${iv_opt} ${lambda_opt} \
            ${bias_opt} -i ${TDIR_DHS}/adj.img -ftol ${ftol} \
            ${verbose_opt} > ${TDIR_DHS}/adj.out 2> ${TDIR_DHS}/adj.log
        dhs_err_code=$?
    else
        ${bindir}/thot_dhs_step_by_step_min -va ${vars} ${iv_opt} \
            -i ${TDIR_DHS}/adj.img -ftol ${ftol} \
            ${verbose_opt} > ${TDIR_DHS}/adj.out 2> ${TDIR_DHS}/adj.log
        dhs_err_code=$?
    fi
    cat ${TDIR_DHS}/adj.out >> ${TDIR_DHS}/adj.xval
    err_msg=`tail -1 ${TDIR_DHS}/adj.log`
    if [ "${err_msg}" = "Image for x required!" ]; then
        # A new evaluation of the target function is required
        values=`cat ${TDIR_DHS}/adj.out`
        ${target_func} ${tdir} ${values} >> ${TDIR_DHS}/adj.img || trgfunc_error="yes"

        # Treat error in target function
        if [ "${trgfunc_error}" = "yes" ]; then
            echo "Error while executing ${target_func}" >&2 

            # $TDIR_DHS will not be removed if the program was executed with -debug flag
            if [ "$debug" != "-debug" ]; then
                echo "NOTE: temporary directory $TDIR_DHS will not be removed to allow error diagnosis" >&2
                trap - EXIT # Do not remove temporary files to be able to diagnose the error
            fi

            # Return with error code
            exit 1 
        fi
    else
        # No more evaluations of the target function are required or
        # there was an error (e.g. maximum number of function
        # evaluations exceeded)
        cp ${TDIR_DHS}/adj.out ${outpref}.out
        cp ${TDIR_DHS}/adj.xval ${outpref}.xval
        cp ${TDIR_DHS}/adj.img ${outpref}.img
        cp ${TDIR_DHS}/adj.log ${outpref}.log
        end=1
        
        # Inform about errors if any
        if [ ${dhs_err_code} -ne 0 ]; then
            echo "Downhill simplex algorithm exited with error status, see file ${outpref}.log" >&2
        fi
    fi
done
