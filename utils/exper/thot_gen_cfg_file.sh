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
put_cf_desc_string()
{
    $SED "s@<cf_file_path>@${cf_desc}@"
}

########
get_lm_desc()
{
    _desc=$1
    if [ ${_desc} = ${none} ]; then
        echo ${_desc}
    else
        get_absolute_path ${_desc} $PWD
    fi
}

########
get_tm_desc()
{
    _desc=$1
    if [ ${_desc} = ${none} ]; then
        echo ${_desc}
    else
        get_absolute_path ${_desc} $PWD
    fi
}

########
get_cf_desc()
{
    _desc=$1
    if [ ${_desc} = ${none} ]; then
        echo ${_desc}
    else
        get_absolute_path ${_desc} $PWD
    fi
}

########
if [ $# -ne 2 -a $# -ne 3 ]; then
    echo "Usage: thot_gen_cfg_file <lm_desc> <tm_desc> [<cf_desc>]"
else
    # Define constant
    none="_none_"

    # Read parameters
    lm_desc=`get_lm_desc $1`
    tm_desc=`get_tm_desc $2`
    
    if [ $# -eq 2 ]; then
        cf_desc=${none}
    else
        cf_desc=`get_cf_desc $3`
    fi

    # Check files
    if [ ${tm_desc} != ${none} -a ! -f ${tm_desc} ]; then
        echo "Error! file with translation model descriptor, ${tm_desc}, does not exist" >&2 
        exit 1
    fi

    if [ ${lm_desc} != ${none} -a ! -f ${lm_desc} ]; then
        echo "Error! file with language model descriptor, ${lm_desc}, does not exist" >&2 
        exit 1
    fi

    if [ ${cf_desc} != ${none} -a ! -f ${cf_desc} ]; then
        echo "Error! file with custom features descriptor, ${cf_desc}, does not exist" >&2 
        exit 1
    fi

    # Generate configuration file
    cat ${datadir}/cfg_templates/thot_basic.cfg | \
        put_lm_desc_string | \
        put_tm_desc_string | \
        put_cf_desc_string
fi
