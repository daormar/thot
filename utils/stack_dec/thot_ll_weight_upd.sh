# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# minimize_trg_func

##################
print_desc()
{
    echo "thot_ll_weight_upd written by Daniel Ortiz."
    echo "thot_ll_weight_upd allows to minimize a given target function." 
    echo "type \"thot_ll_weight_upd --help\" to get usage information."
}

##################
version()
{
    echo "thot_ll_weight_upd is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

##################
usage()
{
    echo "thot_ll_weight_upd [-pr <int>] -c <string> -t <string> -r <string>"
    echo "                   [-va <bool> ... <bool>]"
    echo "                   [-n <int>] [-i <int>] [-tdir <string>]"
    echo "                   [-debug] [-v] [--help] [--version]"
    echo " -pr <int>            : Number of processors."
    echo " -c <string>          : Configuration file."
    echo " -t <string>          : File with test sentences."
    echo " -r <string>          : File with reference sentences."
    echo " -va <bool>...<bool>  : Set variable values to be excluded or included."
    echo "                        Each value equal to 0 excludes the variable and"
    echo "                        values equal to 1 include the variable."
    echo " -n <int>             : Size of the n-best lists."
    echo " -i <int>             : Number of iterations."
    echo " -tdir <string>       : Absolute path of a directory for storing temporary"
    echo "                        files. If not given /tmp is used."
    echo " -v                   : Verbose mode."
    echo " -debug               : After ending, do not delete temporary files"
    echo "                        (for debugging purposes)."
    echo " --help               : Display this help and exit."
    echo " --version            : Output version information and exit."
}

##################
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

##################
str_is_option()
{
    echo "" | ${AWK} -v s=$1 '{if(!match(s,"-[a-zA-Z]")) print "0"; else print "1"}' 
}

##################
get_sentid()
{
    local_file=$1
    echo ${local_file} | $AWK -F "." '{printf"%s",$1}' | $AWK -F "_" '{printf"%s",$2}'
}

##################
obtain_cfg_llweights()
{
    local_line=`$bindir/thot_get_ll_weights -c $cfgfile`
    local_cfg_llw_names=`echo ${local_line} | $AWK '{for(i=1;i<=NF;i+=3) printf"%s ",substr($i,1,length($i)-1)}'`
    local_cfg_llw_values=`echo ${local_line} | $AWK '{for(i=2;i<=NF;i+=3) printf"%s ",$i}'`
    echo ${local_cfg_llw_values}
}

##################
merge_cfg_and_va_weights()
{
    echo ${local_cfg_llweights} | \
        $AWK -v va_values="${va_values}" 'BEGIN{
                                             va_arr_len=split(va_values,va_arr)
                                           }
                                           {
                                             for(i=1;i<=NF;++i)
                                             { 
                                               if(i<=va_arr_len)
                                               {
                                                 if(va_arr[i]=="0")
                                                   printf"0 "
                                                 else
                                                   printf"%s ",$i
                                               }
                                               else
                                               {
                                                 printf"%s ",$i
                                               }
                                             }
                                           }'
}

##################
obtain_init_llweights()
{
    local_cfg_llweights="`obtain_cfg_llweights`"

    if [ ${va_given} -eq 1 ]; then
        local_init_llweights=`merge_cfg_and_va_weights`
    else
        local_init_llweights=${local_cfg_llweights}
    fi

    echo "${local_init_llweights}"
}

##################
gen_nbest_lists_iter()
{
    # Generate translations and word graphs
    $bindir/thot_decoder -pr ${nprocs} -c ${cfgfile} -tmw ${llweights} -t ${testfile} \
        -o ${TDIR_LLWU}/trans/thot_decoder_out_${niter} -wg ${TDIR_LLWU}/wg/${niter} || exit 1

    # Evaluate translation quality
    $bindir/thot_calc_bleu -r ${reffile} -t ${TDIR_LLWU}/trans/thot_decoder_out_${niter} > ${TDIR_LLWU}/trans/thot_decoder_out_${niter}.bleu || exit 1

    # Obtain n-best lists from word graphs
    for wgfile in ${TDIR_LLWU}/wg/${niter}*.wg; do
        basewgfile=`$BASENAME $wgfile`
        sentid=`get_sentid ${basewgfile}`
        ${bindir}/thot_wg_proc -w $wgfile -n ${n_val} -o ${TDIR_LLWU}/nblist/${niter}_${sentid} 2>> ${TDIR_LLWU}/nblist/thot_wg_proc.log || exit 1
    done
}

##################
obtain_trans_quality_from_nblists()
{
    # Take parameters
    local_pref=$1
    local_outfile=$2

    # Obtain best translations for current iteration
    for nblfile in ${local_pref}*.nbl; do
        ${bindir}/thot_obtain_best_trans_from_nbl $nblfile "${llweights}" >> ${local_outfile}
    done
            
    # Calculate translation quality
    local_quality=`$bindir/thot_calc_bleu -r ${reffile} -t ${local_outfile} | $AWK '{printf"%s",$2}'`

    echo ${local_quality}
}

##################
obtain_curr_nblists()
{
    if [ $niter -eq 1 ]; then
        # During first iteration, current n-best list are those
        # generated for initial translation
        for nblfile in ${TDIR_LLWU}/nblist/${niter}*.nbl; do
            basenblfile=`$BASENAME $nblfile`
            sentid=`get_sentid ${basenblfile}`
            cp $nblfile ${TDIR_LLWU}/curr_nblist/$sentid.nbl
        done
    else
        # During subsequent iterations, current n-best lists are
        # obtained by merging n-best lists of the previous iteration
        # with those of the current translation
        for nblfile in ${TDIR_LLWU}/nblist/${niter}*.nbl; do
            basenblfile=`$BASENAME $nblfile`
            sentid=`get_sentid ${basenblfile}`
            ${bindir}/thot_merge_nbest_list $nblfile ${TDIR_LLWU}/curr_nblist/$sentid.nbl > temp.nbl || exit 1
            mv temp.nbl ${TDIR_LLWU}/curr_nblist/$sentid.nbl
        done
    fi
}

##################
proc_curr_nblists()
{
    # Generate file containing file names of current n-best lists
    if [ ! -f ${TDIR_LLWU}/nbl_files.txt ]; then
        for file in ${TDIR_LLWU}/curr_nblist/*nbl; do
            echo $file >> ${TDIR_LLWU}/nbl_files.txt
        done
    fi
    
    # Update weights given n-best lists
    $bindir/thot_ll_weight_upd_nblist -w ${llweights} ${va_opt} \
        -nb ${TDIR_LLWU}/nbl_files.txt -r ${reffile} >> ${TDIR_LLWU}/weights_per_iter.txt 2>${TDIR_LLWU}/${niter}_thot_ll_weight_upd_nblist.log || exit 1
}

##################
get_new_llweights()
{
    $TAIL -1 ${TDIR_LLWU}/weights_per_iter.txt | $AWK '{for(i=3;i<=NF;++i){printf"%s",$i; if(i!=NF) printf" "}}'
}
    
##################

if [ $# -eq 0 ]; then
    print_desc
    exit 1
fi

# Initialize variables
nprocs=1
c_given=0
t_given=0
r_given=0
va_given=0
va_opt=""
n_given=0
n_val=100
i_given=0
maxiters=10
verbose_opt=""
tdir=""
debug=""

# Read parameters
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
        "-pr") shift
            if [ $# -ne 0 ]; then
                nprocs=$1
            fi
            ;;
        "-c") shift
            if [ $# -ne 0 ]; then
                cfgfile=$1
                c_given=1
            fi
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                testfile=$1
                t_given=1
            fi
            ;;
        "-r") shift
            if [ $# -ne 0 ]; then
                reffile=$1
                r_given=1
            fi
            ;;
        "-n") shift
            if [ $# -ne 0 ]; then
                n_val=$1
                n_given=1
            fi
            ;;
        "-i") shift
            if [ $# -ne 0 ]; then
                maxiters=$1
                i_given=1
            fi
            ;;
        "-va") shift
            if [ $# -ne 0 -a ! "`str_is_option $1`" = "1" ]; then
                end=0
                while [ $end -eq 0 ]; do
                    va_values=$va_values" "$1
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
                va_opt="-va ${va_values}"
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

if [ ${c_given} -eq 0 ]; then
    echo "Error: -c option not given" >&2 
    exit 1
else
    if [ ! -f ${cfgfile} ]; then
        echo "Error: file ${cfgfile} does not exist" >&2 
        exit 1        
    fi
fi

if [ ${t_given} -eq 0 ]; then
    echo "Error: -t option not given" >&2 
    exit 1
else
    if [ ! -f ${testfile} ]; then
        echo "Error: file ${testfile} does not exist" >&2 
        exit 1        
    fi
fi

if [ ${r_given} -eq 0 ]; then
    echo "Error: -r option not given" >&2 
    exit 1
else
    if [ ! -f ${reffile} ]; then
        echo "Error: file ${reffile} does not exist" >&2 
        exit 1
    fi
fi

# parameters are ok

# create shared directory
if [ -z "$tdir" ]; then
    # if not given, TDIR_LLWU will be the /tmp directory
    TDIR_LLWU="/tmp/thot_ll_weight_upd_$$"
    mkdir $TDIR_LLWU || { echo "Error: shared directory cannot be created" >&2 ; exit 1; }
else
    TDIR_LLWU="${tdir}/thot_ll_weight_upd_$$"
    mkdir $TDIR_LLWU || { echo "Error: shared directory cannot be created" >&2 ; exit 1; }
fi

# Remove temp directories on exit
if [ "$debug" != "-debug" ]; then
    trap "rm -rf $TDIR_LLWU 2>/dev/null" EXIT
fi

# Create additional directories
mkdir ${TDIR_LLWU}/trans
mkdir ${TDIR_LLWU}/wg
mkdir ${TDIR_LLWU}/nblist
mkdir ${TDIR_LLWU}/curr_nblist

# Obtain initial translation model weights
llweights=`obtain_init_llweights`

# Start iterations
niter=1

while [ $niter -le $maxiters ]; do

    echo "*** Iteration $niter , current weights: ${llweights}" >&2
    
    # Generate n-best lists
    echo "* Generating n-best lists..." >&2
    gen_nbest_lists_iter

    # Obtain translation quality
    quality=`obtain_trans_quality_from_nblists ${TDIR_LLWU}/nblist/${niter} ${TDIR_LLWU}/${niter}_best_trans`
    echo "* Current translation quality: ${quality}" >&2

    # Obtain current n-best lists
    echo "* Obtaining current n-best lists..." >&2
    obtain_curr_nblists

    # Process current n-best lists
    echo "* Processing current n-best lists..." >&2
    proc_curr_nblists

    # Get new log-linear model weights
    llweights=`get_new_llweights`

    # Increase niter
    niter=`expr $niter + 1`

    echo "" >&2

done

# Obtain final quality from current n-best lists
quality=`obtain_trans_quality_from_nblists ${TDIR_LLWU}/curr_nblist/ ${TDIR_LLWU}/curr_nblist_best_trans`
echo "* Final translation quality calculated from current n-best lists: ${quality}" >&2

# Print result
echo ${llweights}
