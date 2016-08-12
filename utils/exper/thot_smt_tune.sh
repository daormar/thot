# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Executes language and translation model tuning.

# \textbf{Categ}: modelling

########
print_desc()
{
    echo "thot_smt_tune written by Daniel Ortiz"
    echo "thot_smt_tune tunes the decoder parameters"
    echo "type \"thot_smt_tune --help\" to get usage information"
}

########
version()
{
    echo "thot_smt_tune is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_smt_tune           [-pr <int>] -c <string>"
    echo "                        -s <string> -t <string> -o <string>"
    echo "                        [-qs <string>] [-tdir <string>]"
    echo "                        [-debug] [--help] [--version]"
    echo ""
    echo "-pr <int>               Number of processors (1 by default)"
    echo "-c <string>             Configuration file"
    echo "-s <string>             File with source sentences"
    echo "-t <string>             File with target sentences"
    echo "-o <string>             Output directory common to all processors."
    echo "-qs <string>            Specific options to be given to the qsub"
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
    echo "-tdir <string>          Directory for temporary files (/tmp by default)."
    echo "                        NOTES:"
    echo "                         a) give absolute paths when using pbs clusters"
    echo "                         b) ensure there is enough disk space in the partition"
    echo "-sdir <string>          Absolute path of a directory common to all"
    echo "                        processors. If not given, \$HOME will be used."
    echo "                        NOTES:"
    echo "                         a) give absolute paths when using pbs clusters"
    echo "                         b) ensure there is enough disk space in the partition"
    echo "-debug                  After ending, do not delete temporary files"
    echo "                        (for debugging purposes)"
    echo "--help                  Display this help and exit."
    echo "--version               Output version information and exit."
    echo ""
    echo "NOTE: When executing the tool in PBS clusters, it is required that the"
    echo "      configuration file and all the files pointed by it are stored in"
    echo "      a place visible to all processors."
}

########
is_absolute_path()
{
    case $1 in
        /*) echo 1 ;;
        *) echo 0 ;;
    esac
}

########
get_absolute_path()
{
    file=$1
    # Check if an absolute path was given
    absolute=`is_absolute_path $file`
    if [ $absolute -eq 1 ]; then
        echo $file
    else
        oldpwd=$PWD
        basetmp=`$BASENAME $PWD/$file`
        dirtmp=`$DIRNAME $PWD/$file`
        cd $dirtmp
        result=${PWD}/${basetmp}
        cd $oldpwd
        echo $result
    fi
}

########
check_if_file_is_desc()
{
    file=$1
    if [ ! -f $file ]; then
        # File does not exist
        echo 0
    else
        # File exists
        nl=`$HEAD -1 $file | $GREP -e "thot lm descriptor" -e "thot tm descriptor" | $WC -l`
        if [ $nl -eq 0 ]; then
            echo 0
        else
            echo 1
        fi
    fi
}

########
create_lm_files()
{
    # Obtain path of lm file
    lmfile=`$GREP "\-lm " $cmdline_cfg | $AWK '{printf"%s",$2}'`

    # Check that lm file could be obtained
    if [ -z "$lmfile" ]; then
        echo "Error! configuration file seems to be wrong"
        exit 1
    fi

    baselmfile=`basename $lmfile`

    # Create directory for lm files
    if [ -d ${outd}/lm ]; then
        echo "Warning! directory for language model does exist" >&2 
    else
        mkdir -p ${outd}/lm || { echo "Error! cannot create directory for language model" >&2; return 1; }
    fi

    # Check if lm file is a descriptor
    is_desc=`check_if_file_is_desc ${lmfile}`

    if [ ${is_desc} -eq 1 ]; then
        # TBD
        echo TBD
    else
        # Create main directory
        if [ ! -d ${outd}/lm/main ]; then
            mkdir ${outd}/lm/main || { echo "Error! cannot create directory for translation model" >&2; return 1; }
        fi

        # Check availability of lm files
        nlines=`ls ${lmfile}* 2>/dev/null | $WC -l`
        if [ $nlines -eq 0 ]; then
            echo "Error! language model files could not be found: ${lmfile}"
            exit 1
        fi

        # Create lm files
        for file in ${lmfile}*; do
            if [ $file = ${lmfile}.weights ]; then
                # Create regular file for the weights
                cp ${lmfile}.weights ${outd}/lm/main || { echo "Error while preparing language model files" >&2 ; return 1; }
            else
                # Create hard links for the rest of the files
                $LN -f $file ${outd}/lm/main || { echo "Error while preparing language model files" >&2 ; return 1; }
            fi
            #        cp $file ${outd}/lm/main || { echo "Error while preparing language model files" >&2 ; return 1; }
        done
        
        # Obtain new lm file name
        newlmfile=${outd}/lm/main/${baselmfile}
        relative_newlmfile=main/${baselmfile}

        # Create descriptor
        echo "thot lm descriptor" > ${outd}/lm/lm_desc
        echo "jm ${relative_newlmfile} main" >> ${outd}/lm/lm_desc
    fi
}

########
lm_downhill_fast()
{
    # Execute tuning algorithm
    ${bindir}/thot_lm_weight_upd -lm $newlmfile -c $tcorpus -v 2> ${outd}/lmweights_tune.log || return 1
}

########
lm_downhill()
{
    # Export required variables
    export LM=$newlmfile
    export TEST=$tcorpus
    export ORDER=`cat $lmfile.weights | $AWK '{printf"%d",$1}'`
    export NUMBUCK=`cat $lmfile.weights | $AWK '{printf"%d",$2}'`
    export BUCKSIZE=`cat $lmfile.weights | $AWK '{printf"%d",$3}'`
    export QS="${qs_par}"

    # Generate information for weight initialisation
    va_opt=`${bindir}/thot_gen_init_file_with_jmlm_weights ${ORDER} ${NUMBUCK} ${BUCKSIZE} -0 | $AWK '{for(i=4;i<=NF;++i) printf"%s ",$i}'`
    iv_opt=`${bindir}/thot_gen_init_file_with_jmlm_weights ${ORDER} ${NUMBUCK} ${BUCKSIZE} 0.5 | $AWK '{for(i=4;i<=NF;++i) printf"%s ",$i}'`

    # Execute tuning algorithm
    ${bindir}/thot_dhs_min -tdir $sdir -va ${va_opt} -iv ${iv_opt} \
        -ftol ${ftol_lm} -o ${outd}/lmweights_tune -u ${bindir}/thot_dhs_trgfunc_jmlm ${debug_opt} || return 1
}

########
tune_lm()
{    
    # Create initial lm files
    create_lm_files || return 1

    # Define boolean variable to check if weight file exists
    if [ -f ${lmfile}.weights ]; then
        weight_file_exists=1
    else
        weight_file_exists=0
    fi

    # Tune language model if weight file was found
    if [ ${weight_file_exists} -eq 1 ]; then
        if [ $DISABLE_FAST_DHSLM -eq 1 ]; then
            lm_downhill || return 1
        else
            lm_downhill_fast || return 1
        fi
    else
        echo "Warning! Language model weights will not be adjusted (unable to find ${lmfile}.weights file)" >&2 
    fi
}

########
create_tm_dev_files()
{
    # Obtain path of tm file
    tmfile=`$GREP "\-tm " $cmdline_cfg | $AWK '{printf"%s",$2}'`

    # Check that tm file could be obtained
    if [ -z "$tmfile" ]; then
        echo "Error! configuration file seems to be wrong"
        exit 1
    fi

    basetmfile=`basename $tmfile`

    # Create directory for tm files for development corpus
    if [ -d ${outd}/tm_dev ]; then
        echo "Warning! directory for dev. translation model does exist" >&2 
    else
        mkdir -p ${outd}/tm_dev || { echo "Error! cannot create directory for translation model" >&2; return 1; }
    fi

    # Check if tm file is a descriptor
    is_desc=`check_if_file_is_desc ${tmfile}`

    if [ ${is_desc} -eq 1 ]; then
        # TBD
        echo TBD
    else
        # Create main directory
        if [ ! -d ${outd}/tm_dev/main ]; then
            mkdir ${outd}/tm_dev/main || { echo "Error! cannot create directory for translation model" >&2; return 1; }
        fi

        # Check availability of tm files
        nlines=`ls ${tmfile}* 2>/dev/null | $WC -l`
        if [ $nlines -eq 0 ]; then
            echo "Error! translation model files could not be found: ${tmfile}"
            exit 1
        fi

        # Create tm files
        for file in ${tmfile}*; do
            # Copy all files except the phrase table
            if [ $file != ${tmfile}.ttable ]; then
                if [ $file = ${tmfile}.lambda ]; then
                    # Create regular file for lambda values
                    cp $file ${outd}/tm_dev/main || { echo "Error while preparing translation model files" >&2 ; return 1; }
                else
                    # Create hard links for the rest of the files
                    $LN -f $file ${outd}/tm_dev/main || { echo "Error while preparing translation model files" >&2 ; return 1; }
                fi
            fi
        done
        
        # Obtain new tm file name for development corpus
        newtmdevfile=${outd}/tm_dev/main/${basetmfile}
        relative_newtmdevfile=main/${basetmfile}

        # Create descriptor
        echo "thot tm descriptor" > ${outd}/tm_dev/tm_desc
        echo "${relative_newtmdevfile} main" >> ${outd}/tm_dev/tm_desc
    fi
}

########
create_tm_files()
{
    # Obtain path of tm file
    tmfile=`$GREP "\-tm " $cmdline_cfg | $AWK '{printf"%s",$2}'`

    # Check that tm file could be obtained
    if [ -z "$tmfile" ]; then
        echo "Error! configuration file seems to be wrong"
        exit 1
    fi

    basetmfile=`basename $tmfile`

    # Create directory for tm files
    if [ -d ${outd}/tm ]; then
        echo "Warning! directory for translation model does exist" >&2 
    else
        mkdir -p ${outd}/tm || { echo "Error! cannot create directory for translation model" >&2; return 1; }
    fi

    # Check if tm file is a descriptor
    is_desc=`check_if_file_is_desc ${tmfile}`

    if [ ${is_desc} -eq 1 ]; then
        # TBD
        echo TBD
    else
        # Create main directory
        if [ ! -d ${outd}/tm/main ]; then
            mkdir ${outd}/tm/main || { echo "Error! cannot create directory for translation model" >&2; return 1; }
        fi

        # Check availability of tm files
        nlines=`ls ${tmfile}* 2>/dev/null | $WC -l`
        if [ $nlines -eq 0 ]; then
            echo "Error! translation model files could not be found: ${tmfile}"
            exit 1
        fi

        # Create tm files
        for file in ${tmfile}*; do
            if [ $file = ${tmfile}.lambda ]; then
                # Synchronize lambda files for tm and tm_dev
                basetmfile=`basename $file`
                $LN -f ${outd}/tm_dev/main/$basetmfile ${outd}/tm/main || { echo "Error while preparing translation model files" >&2 ; return 1; }
            else
                # Create hard links for each file except for that with lambda values
                $LN -f $file ${outd}/tm/main || { echo "Error while preparing translation model files" >&2 ; return 1; }
            fi
        done

        # Obtain new tm file name
        newtmfile=${outd}/tm/main/${basetmfile}
        relative_newtmfile=main/${basetmfile}

        # Create descriptor
        echo "thot tm descriptor" > ${outd}/tm/tm_desc
        echo "${relative_newtmfile} main" >> ${outd}/tm/tm_desc
    fi
}

########
filter_ttable()
{
    ${bindir}/thot_pbs_filter_ttable -t ${tmfile}.ttable \
        -c $scorpus -n 20 -T $tdir ${qs_opt} "${qs_par}" -o ${outd}/tm_dev/main/${basetmfile}.ttable
}

########
create_cfg_file_for_tuning()
{
    $AWK -v nlm=$newlmfile -v ntm=$newtmdevfile \
                         '{
                           if($1=="-lm") $2=nlm
                           if($1=="-tm") $2=ntm
                           printf"%s\n",$0
                          }' $cmdline_cfg
}

##################
obtain_smtweights_names()
{
    local_line=`${bindir}/thot_get_ll_weights | $HEAD -1`
    local_smtw_names=`echo ${local_line} | $AWK '{for(i=5;i<=NF;i+=3) printf"%s ",substr($i,1,length($i)-1)}'`
    echo ${local_smtw_names}
}

########
obtain_loglin_nonneg_const()
{
    local_smtw_names=`obtain_smtweights_names`
    echo "${local_smtw_names}" | $AWK '{for(i=1;i<=NF;++i) if($i=="wp" || $i=="tseglenw") printf"0 "; else printf"1 "}'
}

########
obtain_loglin_dhs_va_opt_values()
{
    local_smtw_names=`obtain_smtweights_names`
    echo "${local_smtw_names}" | $AWK '{for(i=1;i<=NF;++i) if($i=="swlenli") printf"0 "; else printf"-0 "}'
}

########
obtain_loglin_iv_opt_values()
{
    local_smtw_names=`obtain_smtweights_names`
    echo "${local_smtw_names}" | $AWK '{for(i=1;i<=NF;++i) printf"1 "}'
}

########
loglin_downhill()
{
    # Export required variables
    export CFGFILE=${outd}/tune_loglin.cfg
    export TEST=$scorpus
    export REF=$tcorpus
    export PHRDECODER=${bindir}/thot_decoder
    # export PHRDECODER=${bindir}/thot_dec_ms
    export ADD_DEC_OPTIONS="-pr ${pr_val} -sdir $sdir"
    export QS="${qs_par}"
    export USE_NBEST_OPT=0

    # Generate information for weight initialisation
    export NON_NEG_CONST="`obtain_loglin_nonneg_const`"
    va_opt="`obtain_loglin_dhs_va_opt_values`"
    iv_opt="`obtain_loglin_iv_opt_values`"

    # Execute tuning algorithm
    ${bindir}/thot_dhs_min -tdir $sdir -va ${va_opt} -iv ${iv_opt} \
        -ftol ${ftol_loglin} -o ${outd}/llweights_tune -u ${bindir}/thot_dhs_smt_trgfunc ${debug_opt} || return 1
}

########
obtain_loglin_upd_va_opt_values()
{
    local_smtw_names=`obtain_smtweights_names`
#    echo "${local_smtw_names}" | $AWK '{for(i=1;i<=NF;++i) if($i=="swlenli") printf"0 "; else printf"1 "}'
    echo "${local_smtw_names}" | $AWK '{for(i=1;i<=NF;++i) printf"1 "}'
}

########
loglin_upd()
{
    # Generate information for weight initialisation
    va_opt="`obtain_loglin_upd_va_opt_values`"

    # Default parameters
    ll_wu_niters=10

    echo "NOTE: see file ${outd}/llweights_tune.log to track optimization progress" >&2

    # Execute weight update algorithm
    ${bindir}/thot_ll_weight_upd -pr ${pr_val} -va ${va_opt} \
        -c ${outd}/tune_loglin.cfg -t $scorpus -r $tcorpus -i ${ll_wu_niters} \
        ${qs_opt} "${qs_par}" -tdir $tdir -sdir $sdir ${debug_opt} \
        > ${outd}/llweights_tune.out 2> ${outd}/llweights_tune.log || return 1
}

########
linear_interp_upd()
{
    ${bindir}/thot_li_weight_upd -tm ${newtmdevfile} -t $scorpus -r $tcorpus -v \
        2> ${outd}/liweights_tune.log || return 1
}

########
create_cfg_file_for_tuned_sys(){
    # Obtain log-linear weights
    smtweights=`cat ${outd}/llweights_tune.out`

    # Print data regarding development files
    echo "# [SCRIPT_INFO] tool: thot_smt_tune"
    echo "# [SCRIPT_INFO] source dev. file: $scorpus" 
    echo "# [SCRIPT_INFO] target dev. file: $tcorpus" 
    echo "# [SCRIPT_INFO] initial cfg file: $cmdline_cfg"
    echo "# [SCRIPT_INFO]"

    # Create file from command line file
    cat ${outd}/tune_loglin.cfg | $SED s'@/tm_dev/@/tm/@'| \
        $AWK -v smtweights="$smtweights" \
                            '{
                               if($1=="#" && $2=="-tmw")
                               {
                                 printf"-tmw %s\n",smtweights
                               }
                               else printf "%s\n",$0
                             }'
}

########
tune_tm()
{
    # Create directory for lm files
    if [ -d ${outd}/lm ]; then
        lm_dir_already_exist=1
    else
        mkdir -p ${outd}/lm || { echo "Error! cannot create directory for language model" >&2; return 1; }
        lm_dir_already_exist=0
    fi

    if [ ${lm_dir_already_exist} -eq 0 ]; then
        # Create initial lm files
        create_lm_files
    fi

    ######

    # Create initial tm_dev files
    create_tm_dev_files || return 1

    # Filter translation table
    echo "" >&2
    echo "- Filtering translation table for development corpus..." >&2
    filter_ttable || return 1

    # Tune linear interpolation weights
    if [ $ENABLE_UPDATE_LIWEIGHTS -eq 1 ]; then
        echo "" >&2
        echo "- Tuning phrase model linear interpolation weights..." >&2
        linear_interp_upd || return 1
    fi

    # Create cfg file for tuning
    create_cfg_file_for_tuning > ${outd}/tune_loglin.cfg

    # Tune log-linear model
    echo "" >&2
    echo "- Tuning log-linear model weights..." >&2
    if [ $ENABLE_DOWNHILL_LLW -eq 1 ]; then
        loglin_downhill || return 1
    else
        loglin_upd || return 1
    fi

    ######

    # Create initial tm files
    create_tm_files || return 1

    # Create cfg file of tuned system
    create_cfg_file_for_tuned_sys > ${outd}/tuned_for_dev.cfg
}

########
if [ $# -lt 1 ]; then
    print_desc
    exit 1
fi

# Read parameters
pr_given=0
pr_val=1
c_given=0
s_given=0
t_given=0
o_given=0
qs_given=0
unk_given=0
tdir_given=0
tdir="/tmp"
sdir_given=0
sdir=$HOME
debug=0

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
        "-c") shift
            if [ $# -ne 0 ]; then
                cmdline_cfg=$1
                c_given=1
            fi
            ;;
        "-s") shift
            if [ $# -ne 0 ]; then
                scorpus=$1
                s_given=1
            fi
            ;;
        "-t") shift
            if [ $# -ne 0 ]; then
                tcorpus=$1
                t_given=1
            fi
            ;;
        "-o") shift
            if [ $# -ne 0 ]; then
                outd=$1
                o_given=1
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
        "-unk") unk_given=1
            unk_opt="-unk"
            ;;
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir=$1
                tdir_given=1
            fi
            ;;
        "-sdir") shift
            if [ $# -ne 0 ]; then
                sdir=$1
                sdir_given=1
            fi
            ;;
        "-debug") debug=1
            debug_opt="-debug"
            ;;
    esac
    shift
done

# Check parameters
if [ ${c_given} -eq 0 ]; then
    echo "Error! -c parameter not given" >&2
    exit 1
else
    if [ ! -f ${cmdline_cfg} ]; then
        echo "Error! file ${cmdline_cfg} does not exist" >&2
        exit 1
    else
        # Obtain absolute path
        cmdline_cfg=`get_absolute_path ${cmdline_cfg}`
    fi
fi

if [ ${s_given} -eq 0 ]; then
    echo "Error! -s parameter not given!" >&2
    exit 1
else
    if [ ! -f ${scorpus} ]; then
        echo "Error! file ${scorpus} does not exist" >&2
        exit 1
    else
        # Obtain absolute path
        scorpus=`get_absolute_path $scorpus`
    fi
fi

if [ ${t_given} -eq 0 ]; then        
    echo "Error! -t parameter not given!" >&2
    exit 1
else
    if [ ! -f ${tcorpus} ]; then
        echo "Error! file ${tcorpus} does not exist" >&2
        exit 1
    else
        # Obtain absolute path
        tcorpus=`get_absolute_path $tcorpus`
    fi
fi

# Check that source and target files are parallel
nl_source=`wc -l $scorpus | $AWK '{printf"%d",$1}'`
nl_target=`wc -l $tcorpus | $AWK '{printf"%d",$1}'`

if [ ${nl_source} -ne ${nl_target} ]; then
    echo "Error! source and target files have not the same number of lines" >&2 
    exit 1
fi

if [ ${o_given} -eq 0 ]; then
    echo "Error! -o parameter not given!" >&2
    exit 1
else
    if [ -d ${outd} ]; then
        echo "Warning! output directory does exist" >&2 
        # echo "Error! output directory should not exist" >&2 
        # exit 1
    else
        mkdir -p ${outd} || { echo "Error! cannot create output directory" >&2; return 1; }
    fi
    # Obtain absolute path
    outd=`get_absolute_path $outd`
fi

if [ ${tdir_given} -eq 1 ]; then
    if [ ! -d ${tdir} ]; then
        echo "Error! directory ${tdir} does not exist" >&2
        exit 1            
    fi
fi

if [ ${sdir_given} -eq 1 ]; then
    if [ ! -d ${sdir} ]; then
        echo "Error! directory ${sdir} does not exist" >&2
        exit 1            
    fi
fi

# Set default parameters
ftol_lm=0.1
ftol_loglin=0.001

# Tune models
echo "* Tuning language model..." >&2
tune_lm || exit 1
echo "" >&2

echo "* Tuning translation model..." >&2
tune_tm || exit 1
echo "" >&2
