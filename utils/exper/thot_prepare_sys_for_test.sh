# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Generates a server configuration file and the necessary files to
# translate a given test corpus.

########
print_desc()
{
    echo "thot_prepare_sys_for_test written by Daniel Ortiz"
    echo "thot_prepare_sys_for_test prepares all the necessary to translate a test corpus"
    echo "type \"thot_prepare_sys_for_test --help\" to get usage information"
}

########
version()
{
    echo "thot_prepare_sys_for_test is part of the thot package"
    echo "thot version "${version}
    echo "thot is GNU software written by Daniel Ortiz"
}

########
usage()
{
    echo "thot_prepare_sys_for_test -c <string> -t <string> -o <string>"
    echo "                          [-qs <string>] [-tdir <string>] [-sdir <string>]"
    echo "                          [--help] [--version]"
    echo ""
    echo "-c <string>             Configuration file"
    echo "-t <string>             File with test sentences"
    echo "-o <string>             Output directory common to all processors."
    echo "-qs <string>            Specific options to be given to the qsub command"
    echo "                        (example: -qs \"-l pmem=1gb\")."
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
    echo "--help                  Display this help and exit."
    echo "--version               Output version information and exit."
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
        # Files does not exist
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
process_files_for_individual_lm()
{
    # Initialize parameters
    _lmfile=$1
    _lm_status=$2

    # Create lm directory
    if [ ! -d ${outd}/lm/${_lm_status} ]; then
        mkdir ${outd}/lm/${_lm_status} || { echo "Error! cannot create directory for language model" >&2; return 1; }
    fi
    
    # Check availability of lm files
    nlines=`ls ${_lmfile}* 2>/dev/null | $WC -l`
    if [ $nlines -eq 0 ]; then
        echo "Error! language model files could not be found: ${_lmfile}"
        exit 1
    fi
    
    # Create lm files
    for file in `ls ${_lmfile}*`; do
        # Create hard links for each file
        $LN -f $file ${outd}/lm/${_lm_status} || { echo "Error while preparing language model files" >&2 ; return 1; }
    done

    # Add entry to descriptor file
    _baselmfile=`basename ${_lmfile}`
    _relative_newlmfile=${_lm_status}/${_baselmfile}    
    echo "jm ${_relative_newlmfile} ${_lm_status}" >> ${outd}/lm/lm_desc
}

########
list_lm_entry_info()
{
    lmdesc=$1
    $TAIL -n +2 ${lmdesc} | $AWK '{printf"%s,%s,%s\n",$1,$2,$3}'
}

########
create_lm_files()
{
    # Obtain path of lm file
    lmfile=`$GREP "\-lm " $cmdline_cfg | $AWK '{printf"%s",$2}'`
    baselmfile=`basename $lmfile`

    # Create directory for lm files
    if [ -d ${outd}/lm ]; then
        echo "Warning! directory for language model does exist" >&2 
    else
        mkdir -p ${outd}/lm || { echo "Error! cannot create directory for language model" >&2; return 1; }
    fi

    # Check if tm file is a descriptor
    is_desc=`check_if_file_is_desc ${lmfile}`

    if [ ${is_desc} -eq 1 ]; then
        # Create descriptor
        echo "thot lm descriptor" > ${outd}/lm/lm_desc

        # Create files for the different language models
        lmdesc_dirname=`$DIRNAME $lmfile`
        for lm_entry in `list_lm_entry_info $lmfile`; do
            curr_lmfile=`echo ${lm_entry} | $AWK -F "," '{printf"%s",$2}'`
            curr_status=`echo ${lm_entry} | $AWK -F "," '{printf"%s",$3}'`
            process_files_for_individual_lm ${lmdesc_dirname}/${curr_lmfile} ${curr_status}
        done

        # Copy weights for lm descriptor
        cp ${lmfile}.weights ${outd}/lm/

        # Obtain new file name for lm descriptor
        baselmfile=`basename $lmfile`
        newlmfile=${outd}/lm/${baselmfile}
    else
        # Create descriptor
        echo "thot lm descriptor" > ${outd}/lm/lm_desc

        # Create files for individual language model
        process_files_for_individual_lm ${lmfile} "main"

        # Obtain new lm file name
        baselmfile=`basename $lmfile`
        newlmfile=${outd}/lm/main/${baselmfile}
    fi
}

########
create_tm_files()
{
    # Create directory for tm files
    if [ -d ${outd}/tm ]; then
        echo "Warning! directory for translation model does exist" >&2 
    else
        mkdir -p ${outd}/tm || { echo "Error! cannot create directory for translation model" >&2; return 1; }
    fi

    # Obtain path of tm file
    tmfile=`$GREP "\-tm " $cmdline_cfg | $AWK '{printf"%s",$2}'`
    basetmfile=`basename $tmfile`

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
        for file in `ls ${tmfile}*`; do
            if [ $file != ${tmfile}.ttable ]; then
                # Create hard links for all of the files except the phrase table
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
    # ${bindir}/thot_filter_ttable -t ${tmfile}.ttable \
    #     -c $tcorpus -n 20 -T $tdir > ${outd}/tm/${basetmfile}.ttable 2> ${outd}/tm/main/${basetmfile}.ttable.log
    ${bindir}/thot_pbs_filter_ttable -t ${tmfile}.ttable \
        -c $tcorpus -n 20 ${qs_opt} "${qs_par}" -T $tdir -o ${outd}/tm/main/${basetmfile}.ttable
}

########
generate_cfg_file()
{
    # Print data regarding development files
    echo "# [SCRIPT_INFO] tool: thot_prepare_sys_for_test"
    echo "# [SCRIPT_INFO] test file: $tcorpus" 
    echo "# [SCRIPT_INFO] initial cfg file: $cmdline_cfg"
    echo "# [SCRIPT_INFO]"

    # Create file from command line file
    cat $cmdline_cfg | $AWK -v nlm=$newlmfile -v ntm=$newtmfile \
                         '{
                           if(!($1=="#" && $2=="[SCRIPT_INFO]"))
                           {
                            if($1=="-lm") $2=nlm
                            if($1=="-tm") $2=ntm
                            printf"%s\n",$0
                           }
                          }'
}

########
if [ $# -lt 1 ]; then
    print_desc
    exit 1
fi

# Read parameters
t_given=0
o_given=0
tdir_given=0
tdir="/tmp"
sdir_given=0
sdir=$HOME

while [ $# -ne 0 ]; do
    case $1 in
        "--help") usage
            exit 0
            ;;
        "--version") version
            exit 0
            ;;
        "-c") shift
            if [ $# -ne 0 ]; then
                cmdline_cfg=$1
                c_given=1
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
        "-tdir") shift
            if [ $# -ne 0 ]; then
                tdir="$1"
                tdir_given=1
            fi
            ;;
        "-sdir") shift
            if [ $# -ne 0 ]; then
                sdir=$1
                sdir_given=1
            fi
            ;;
    esac
    shift
done

# Check parameters
if [ ${c_given} -eq 0 ]; then
    echo "Error! -cfg parameter not given" >&2
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

if [ ${t_given} -eq 0 ]; then        
    echo "Error! -t parameter not given" >&2
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

if [ ${o_given} -eq 0 ]; then
    echo "Error! -o parameter not given!" >&2
    exit 1
else
    if [ -d ${outd} ]; then
        echo "Warning! output directory does exist" >&2 
        # echo "Error! output directory should not exist" >&2 
        # exit 1
    else
        mkdir -p ${outd}/ || { echo "Error! cannot create output directory" >&2; return 1; }
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

# Create lm files
create_lm_files || exit 1

# Create tm files
create_tm_files || exit 1

# Filter tm model
filter_ttable || exit 1

# Generate cfg file
generate_cfg_file > ${outd}/test_specific.cfg
