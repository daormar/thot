# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# minimize_trg_func

##################
print_desc()
{
    echo "thot_ll_weight_upd written by Daniel Ortiz."
    echo "thot_ll_weight_upd performs log-linear weight updating." 
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
    echo "                   [-va <bool> ... <bool>] [-n <int>] [-i <int>]"
    echo "                   [-qs <string>] [-sdir <string>] [-sdir <string>]"
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
    echo " -qs <string>         : Specific options to be given to the qsub"
    echo "                        command (example: -qs \"-l pmem=1gb\")"
    echo "                        NOTES:"
    echo "                         a) ignore this if not using a PBS cluster"
    echo "                         b) -qs option may be crucial to ensure the correct"
    echo "                            execution of the tool. The main purpose of -qs"
    echo "                            is to reserve the required cluster resources."
    echo "                            If the necessary resources are not met the"
    echo "                            execution will abort."
    echo "                            Resources are reserved in different ways depending"
    echo "                            on the cluster software. For instance, if using SGE"
    echo "                            software, -qs \"-l h_vmem=1G,h_rt=10:00:00\","
    echo "                            requests 1GB of virtual memory and a time limit"
    echo "                            of 10 hours." 
    echo " -tdir <string>       : Directory for temporary files (/tmp by default)."
    echo "                        NOTES:"
    echo "                         a) give absolute paths when using pbs clusters"
    echo "                         b) ensure there is enough disk space in the partition"
    echo " -sdir <string>       : Absolute path of a directory common to all"
    echo "                        processors. If not given \$HOME is used"
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
    local_line=`$bindir/thot_get_ll_weights -c $cfgfile | $HEAD -1`
    local_cfg_llw_names=`echo ${local_line} | $AWK '{for(i=5;i<=NF;i+=3) printf"%s ",substr($i,1,length($i)-1)}'`
    local_cfg_llw_values=`echo ${local_line} | $AWK '{for(i=6;i<=NF;i+=3) printf"%s ",$i}'`
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
        -o ${TDIR_LLWU}/trans/${niter}_thot_decoder_out -wg ${TDIR_LLWU}/wg/${niter} \
        -sdir $sdir ${qs_opt} "${qs_par}" -v || { trap - EXIT ; return 1; }

    # Evaluate translation quality
    $bindir/thot_scorer -r ${reffile} -t ${TDIR_LLWU}/trans/${niter}_thot_decoder_out > ${TDIR_LLWU}/trans/${niter}_thot_decoder_out.score || return 1

    # Obtain n-best lists from word graphs
    for wgfile in ${TDIR_LLWU}/wg/${niter}*.wg; do
        basewgfile=`$BASENAME $wgfile`
        sentid=`get_sentid ${basewgfile}`
        ${bindir}/thot_wg_proc -w $wgfile -n ${n_val} -o ${TDIR_LLWU}/nblist/${niter}_${sentid} 2>> ${TDIR_LLWU}/nblist/thot_wg_proc.log || return 1
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
    local_quality=`$bindir/thot_scorer -r ${reffile} -t ${local_outfile} | $AWK '{printf"%s",$2}'`

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
            ${bindir}/thot_merge_nbest_list $nblfile ${TDIR_LLWU}/curr_nblist/$sentid.nbl > ${TDIR_LLWU}/temp.nbl || return 1
            mv ${TDIR_LLWU}/temp.nbl ${TDIR_LLWU}/curr_nblist/$sentid.nbl
        done
    fi
}

##################
update_best_quality()
{
    if [ -z "${best_quality}" ]; then
        best_quality=${quality}
        best_llweights=${llweights}
    else
        curr_best_quality_worse=`echo ${best_quality} ${quality} | $AWK '{printf"%d",$1<$2}'`
        if [ ${curr_best_quality_worse} -eq 1 ]; then
            best_quality=${quality}
            best_llweights=${llweights}
        fi
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
        -nb ${TDIR_LLWU}/nbl_files.txt -r ${reffile} >> ${TDIR_LLWU}/weights_per_iter.txt 2>${TDIR_LLWU}/${niter}_thot_ll_weight_upd_nblist.log || return 1
}

##################
get_new_llweights()
{
    $TAIL -1 ${TDIR_LLWU}/weights_per_iter.txt | $AWK '{for(i=3;i<=NF;++i){printf"%s",$i; if(i!=NF) printf" "}}'
}

##################
convergence_reached()
{
    local_trans_qual_vector="$1"
    local_longest_decr_streak=$2
    local_decreasing_streak_len=`echo ${local_trans_qual_vector} | $AWK '{
                                             best_score=$1
                                             decr_streak_len=1
                                             for(i=2;i<=NF;++i)
                                             {
                                               if(best_score<$i)
                                               {
                                                best_score=$i
                                                decr_streak_len=1
                                               }
                                               else
                                               {
                                                ++decr_streak_len
                                               }
                                             }
                                             printf "%d",decr_streak_len
                                            }'`
    if [ ${local_decreasing_streak_len} -ge ${local_longest_decr_streak} ]; then
        echo 1
    else
        echo 0
    fi
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
maxiters=12
verbose_opt=""
sdir=$HOME
tdir=/tmp
qs_given=0
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
        "-sdir") shift
            if [ $# -ne 0 ]; then
                sdir=$1
            fi
            ;;
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1
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
        "-qs") shift
            if [ $# -ne 0 ]; then
                qs_opt="-qs"
                qs_par="$1"
                qs_given=1
            else
                qs_given=0
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

# Check shared directory

if [ ! -d ${sdir} ]; then
    echo "Error: shared directory does not exist" >&2
    return 1;
fi

# Create tmp directory

if [ ! -d ${tdir} ]; then
    echo "Error: tmp directory does not exist" >&2
    return 1;
fi

TDIR_LLWU=`${MKTEMP} -d ${tdir}/thot_ll_weight_upd_XXXXXX`

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
trans_qual_vector=""

while [ 1 ]; do

    echo "*** Iteration $niter , current weights: ${llweights}" >&2
    
    # Generate n-best lists
    echo "* Generating n-best lists..." >&2
    gen_nbest_lists_iter || exit 1

    # Obtain translation quality
    quality=`obtain_trans_quality_from_nblists ${TDIR_LLWU}/nblist/${niter} ${TDIR_LLWU}/${niter}_best_trans`
    trans_qual_vector="${trans_qual_vector} ${quality}"
    echo "* Current translation quality: ${quality}" >&2

    # Update best quality
    update_best_quality || exit 1 

    # Verify ending conditions
    if [ $niter -ge $maxiters ]; then
        echo "" >&2
        break
    fi

    longest_decr_streak=4
    converg=`convergence_reached "${trans_qual_vector}" ${longest_decr_streak}`
    if [ $converg -eq 1 ]; then
        echo "" >&2
        break
    fi

    # Obtain current n-best lists
    echo "* Obtaining current n-best lists..." >&2
    obtain_curr_nblists || exit 1

    # Process current n-best lists
    echo "* Processing current n-best lists..." >&2
    proc_curr_nblists || exit 1

    # Get new log-linear model weights
    llweights=`get_new_llweights`
    echo "* New weights: ${llweights}" >&2
        
    # Obtain translation quality for new weights
    if [ "$debug" = "-debug" ]; then
        quality=`obtain_trans_quality_from_nblists ${TDIR_LLWU}/nblist/${niter} ${TDIR_LLWU}/${niter}_best_trans_new_weights`
        echo "* Translation quality for new weights: ${quality}" >&2
    fi
    
    # Increase niter
    niter=`expr $niter + 1`
    
    echo "" >&2
    
done

# Print result
echo "* Best weights: ${best_llweights}" >&2
echo ${best_llweights}
