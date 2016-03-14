# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Extract hypothesis information for each source sentence given a
# .dec_err file generated with thot_decoder

if [ $# -ne 1 ]; then
    echo "Usage: thot_extract_hyp_info <thot_decoder_dec_err_file>"
else

    file=$1
    ${GREP} "^Score: " $file | $AWK '{if($3==";") printf"%s\n",$0}'
 
fi
