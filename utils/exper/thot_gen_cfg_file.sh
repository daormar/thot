# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Generates a thot configuration file given translation and language
# model descriptors.

if [ $# -ne 2 ]; then
    echo "Usage: thot_gen_cfg_file <lm_desc> <tm_desc>"
else
    # Read parameters
    lm_desc=$1
    tm_desc=$2

    # Check files
    if [ ! -f ${tm_desc} ]; then
        echo "Error! file with translation model descriptor, ${tm_desc}, does not exist" >&2 
        exit 1
    fi

    if [ ! -f ${lm_desc} ]; then
        echo "Error! file with language model descriptor, ${lm_desc}, does not exist" >&2 
        exit 1
    fi

    # Obtain lm file path
    lmfile=`$HEAD -2 ${lm_desc} | $TAIL -1 | $AWK '{printf"%s",$2}'`

    # Obtain tm file path
    tmfile=`$HEAD -2 ${tm_desc} | $TAIL -1 | $AWK '{printf"%s",$1}'`

    # Generate configuration file
    cat ${datadir}/cfg_templates/thot_basic.cfg | \
        $SED "s@<lm_file_path>@${lmfile}@" | \
        $SED "s@<tm_file_path>@${tmfile}@"
fi
