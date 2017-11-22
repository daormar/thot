# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Generates a thot configuration file given translation and language
# model descriptors.

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
    # Initialize variables
    file=$1
    filedir=$2

    # Check if an absolute path was given
    absolute=`is_absolute_path $file`
    if [ $absolute -eq 1 ]; then
        echo $file
    else
        oldpwd=$PWD
        basetmp=`$BASENAME ${filedir}/$file`
        dirtmp=`$DIRNAME ${filedir}/$file`
        cd $dirtmp
        result=${PWD}/${basetmp}
        cd $oldpwd
        echo $result
    fi
}

########
put_lm_desc_string()
{
    $SED "s@<lm_file_path>@${lm_desc}@"
}

########
put_tm_desc_string()
{
    $SED "s@<tm_file_path>@${tm_desc}@"
}

########
if [ $# -ne 2 ]; then
    echo "Usage: thot_gen_cfg_file <lm_desc> <tm_desc>"
else
    # Read parameters
    lm_desc=`get_absolute_path $1 $PWD`
    tm_desc=`get_absolute_path $2 $PWD`

    # Initialize variables
    lm_desc_dir=`$DIRNAME ${lm_desc}`
    tm_desc_dir=`$DIRNAME ${tm_desc}`

    # Check files
    if [ ! -f ${tm_desc} ]; then
        echo "Error! file with translation model descriptor, ${tm_desc}, does not exist" >&2 
        exit 1
    fi

    if [ ! -f ${lm_desc} ]; then
        echo "Error! file with language model descriptor, ${lm_desc}, does not exist" >&2 
        exit 1
    fi

    # Generate configuration file
    cat ${datadir}/cfg_templates/thot_basic.cfg | \
        put_lm_desc_string | \
        put_tm_desc_string
fi
