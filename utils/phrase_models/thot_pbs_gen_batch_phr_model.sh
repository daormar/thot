# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Trains a phrase-based model given a parallel corpus.

# \textbf{Categ}: modelling

########
if [ $# -lt 8 ]; then
    echo "Usage: thot_pbs_gen_batch_phr_model -pr <int>"
    echo "                                    -s <string> -t <string> -o <string>"
    echo "                                    [-n <int>] [-np <float>] [-lf <float>]"
    echo "                                    [-af <float>] [-cpr <float>] [-m <int>]"
    echo "                                    [-nsh] [-qs <string>] -T <string>"
    echo "                                    -sdir <string> [-debug]"
    echo ""
    echo "-pr <int>               Number of processors"
    echo "-s <string>             File with source sentences (give absolute path when"
    echo "                        using pbs clusters)"
    echo "-t <string>             File with target sentences (give absolute path when"
    echo "                        using pbs clusters)"
    echo "-o <string>             Prefix of the output files (give absolute path when"
    echo "                        using pbs clusters)"
    echo "-n <int>                Number of iterations of the EM algorithm executed by"
    echo "                        the gen_sw_model tool (5 by default)"
    echo "-lf <float>             Lexical smoothing interpolation factor for single-word"
    echo "                        models"
    echo "-af <float>             Alignment smoothing interpolation factor for"
    echo "                        single-word models"
    echo "-np <float>             Probability assigned to the alignment with the NULL"
    echo "                        word for single-word models"
    echo "-cpr <float>            Pruning parameter used during the estimation of single"
    echo "                        word alignment models (0.000001 by default)"
    echo "-m <int>                Maximum target phrase length during phrase model"
    echo "                        estimation (7 by default)"
    echo "-nsh                    Do not shuffle load during training (shuffle is"
    echo "                        important to achieve load balancing)"
    echo "-qs <string>            Specific options to be given to the qsub"
    echo "                        command (example: -qs \"-l pmem=1gb\")"
    echo "-T <string>             Directory for temporary files."
    echo "                        NOTES:"
    echo "                         a) give absolute paths when using pbs clusters."
    echo "                         b) ensure there is enough disk space in the partition"
    echo "-sdir <string>          Absolute path of a directory common to all"
    echo "                        processors."
    echo "                        NOTES:"
    echo "                         a) give absolute paths when using pbs clusters"
    echo "                         b) ensure there is enough disk space in the partition"
    echo "-debug                  After ending, do not delete temporary files"
    echo "                        (for debugging purposes)"

else
    # Read parameters
    pr_given=0
    s_given=0
    t_given=0
    o_given=0
    n_given=0
    lf_given=0
    af_given=0
    np_given=0
    cpr_given=0
    cpr_default_val=0.000001
    niters=5
    m_val=7
    qs_given=0
    nsh_given=0
    shuff_opt="-shu"
    tdir_given=0
    sdir_given=0
    debug=0
    lambda_default_val=0.01
    sslen_default_val="Uniform"
    tslen_default_val="Geometric"

    while [ $# -ne 0 ]; do
        case $1 in
            "-pr") shift
                if [ $# -ne 0 ]; then
                    pr_val=$1
                    pr_given=1
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
                    outp=$1
                    o_given=1
                fi
                ;;
            "-n") shift
                if [ $# -ne 0 ]; then
                    niters=$1
                    n_given=1
                fi
                ;;
            "-lf") shift
                if [ $# -ne 0 ]; then
                    lf_opt="-lf $1"
                    lf_given=1
                else
                    lf_given=0
                fi
                ;;
            "-af") shift
                if [ $# -ne 0 ]; then
                    af_opt="-af $1"
                    af_given=1
                else
                    af_given=0
                fi
                ;;
            "-np") shift
                if [ $# -ne 0 ]; then
                    np_opt="-np $1"
                    np_given=1
                else
                    np_given=0
                fi
                ;;
            "-cpr") shift
                if [ $# -ne 0 ]; then
                    cpr_opt="-cpr $1"
                    cpr_given=1
                else
                    cpr_opt="-cpr ${cpr_default_val}"
                    cpr_given=0
                fi
                ;;
            "-m") shift
                if [ $# -ne 0 ]; then
                    m_val=$1
                    m_given=1
                fi
                ;;
            "-qs") shift
                if [ $# -ne 0 ]; then
                    qs_opt="-qs"
                    qs_par=$1
                    qs_given=1
                else
                    qs_given=0
                fi
                ;;
            "-nsh") nsh_given=1
                shuff_opt=""
                ;;
            "-T") shift
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

    # Verify parameters

    if [ ${pr_given} -eq 0 ]; then
        # invalid parameters 
        echo "Error: number of processors not given"
        exit 1
    fi

    if [ ${s_given} -eq 0 ]; then
        echo "Error! -s parameter not given!" >&2
        exit 1
    else
        if [ ! -f ${scorpus} ]; then
            echo "Error! file ${scorpus} does not exist" >&2
            exit 1            
        fi
    fi

    if [ ${t_given} -eq 0 ]; then        
        echo "Error! -t parameter not given!" >&2
        exit 1
    else
        if [ ! -f ${tcorpus} ]; then
            echo "Error! file ${tcorpus} does not exist" >&2
            exit 1            
        fi
    fi

    if [ ${o_given} -eq 0 ]; then
        echo "Error! -o parameter not given!" >&2
        exit 1
    fi

    if [ ${tdir_given} -eq 0 ]; then
        echo "Error! -T parameter not given!" >&2
        exit 1
    else
        if [ ! -d ${tdir} ]; then
            echo "Error! directory ${tdir} does not exist" >&2
            exit 1            
        fi
    fi

    if [ ${sdir_given} -eq 0 ]; then
        echo "Error! -sdir parameter not given!" >&2
        exit 1
    else
        if [ ! -d ${sdir} ]; then
            echo "Error! directory ${sdir} does not exist" >&2
            exit 1            
        fi
    fi

    # Train models

    # Generate single word direct and inverse models using sw_models package

    # Generate direct single word model
${bindir}/thot_pbs_gen_batch_sw_model -pr ${pr_val} -s $scorpus -t $tcorpus -n ${niters} ${lf_opt} ${af_opt} ${np_opt} \
        ${cpr_opt} ${shuff_opt} -o ${outp}_swm ${qs_opt} "${qs_par}" -sdir $sdir -tdir $tdir ${debug_opt} 2> /dev/null || exit 1
    # Rename log file
    mv ${outp}_swm.log ${outp}_swm_thot_pbs_gen_batch_sw_model.log

    # Generate best alignments for direct model
${bindir}/thot_pbs_gen_best_sw_alig -pr ${pr_val} -sw ${outp}_swm -s $scorpus -t $tcorpus \
        ${shuff_opt} -o ${outp}_swm ${qs_opt} "${qs_par}" -sdir $sdir -tdir $tdir ${debug_opt} 2> /dev/null
    # Rename log file
    mv ${outp}_swm.log ${outp}_swm_thot_pbs_gen_best_sw_alig.log

    # Generate inverse single word model
${bindir}/thot_pbs_gen_batch_sw_model -pr ${pr_val} -s $tcorpus -t $scorpus -n ${niters} ${lf_opt} ${af_opt} ${np_opt} \
        ${cpr_opt} ${shuff_opt} -o ${outp}_invswm ${qs_opt} "${qs_par}" -sdir $sdir -tdir $tdir ${debug_opt} 2> /dev/null || exit 1
    # Rename log file
    mv ${outp}_invswm.log ${outp}_invswm_thot_pbs_gen_batch_sw_model.log

    # Generate best alignments for inverse model
${bindir}/thot_pbs_gen_best_sw_alig -pr ${pr_val} -sw ${outp}_invswm -s $tcorpus -t $scorpus \
        ${shuff_opt} -o ${outp}_invswm ${qs_opt} "${qs_par}" -sdir $sdir -tdir $tdir ${debug_opt} 2> /dev/null
    # Rename log file
    mv ${outp}_invswm.log ${outp}_invswm_thot_pbs_gen_best_sw_alig.log

    # Operate word alignments generated with the sw_models package
    $bindir/thot_pbs_alig_op -pr ${pr_val} -g ${outp}_swm.bestal -sym1 ${outp}_invswm.bestal -o ${outp}_alig_op \
        ${qs_opt} "${qs_par}" -sdir $sdir -T $tdir ${debug_opt} 2> /dev/null || exit 1
    # Rename log file
    mv ${outp}_alig_op.log ${outp}_thot_pbs_alig_op.log

    # Generate phrase model
    $bindir/thot_pbs_gen_phr_model -pr ${pr_val} -g ${outp}_alig_op.A3.final -m ${m_val} \
        -o ${outp} ${pml_opt} -pc ${qs_opt} "${qs_par}" -sdir $sdir -T $tdir ${debug_opt} 2> ${outp}_thot_pbs_gen_phr_model.log || exit 1

    # Generate additional phrase model parameter files
    echo ${lambda_default_val} > ${outp}.lambda
    echo ${sslen_default_val} > ${outp}.srcsegmlentable
    echo ${tslen_default_val} > ${outp}.trgsegmlentable

    exit 0
fi
