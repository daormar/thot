# Author: Daniel Ortiz Mart\'inez
# *- bash -*

# Trains a phrase-based model given a parallel corpus.

# \textbf{Categ}: modelling

########
treat_unk_opt()
{
    scorpus_tmp=`${MKTEMP} ${sdir}/src.XXXXXX`
    cp ${scorpus} ${scorpus_tmp}
    echo "UNKNOWN_WORD" >> ${scorpus_tmp}
    scorpus=${scorpus_tmp}

    tcorpus_tmp=`${MKTEMP} ${sdir}/trg.XXXXXX`
    cp ${tcorpus} ${tcorpus_tmp}
    echo "UNKNOWN_WORD" >> ${tcorpus_tmp}
    tcorpus=${tcorpus_tmp}

    trap "rm -rf ${scorpus_tmp} ${tcorpus_tmp} 2>/dev/null" EXIT
}

########
empty_train()
{
    # Generate direct and inverse single word models using sw_models package

    # Generate direct single word model
    echo "* Generating source-to-target single word alignment model... " >&2
    ${bindir}/thot_gen_sw_model -s $scorpus -t $tcorpus -n 1 ${lf_opt} ${af_opt} ${np_opt} \
        -o ${outp}_swm 2>/dev/null || exit 1
    echo "0" > ${outp}_swm.msinfo
    echo "" >&2

    # Generate inverse single word model
    echo "* Generating target-to-source single word alignment model... " >&2
    ${bindir}/thot_gen_sw_model -s $tcorpus -t $scorpus -n ${niters} ${lf_opt} ${af_opt} ${np_opt} \
        -o ${outp}_invswm 2>/dev/null || exit 1
    echo "0" > ${outp}_invswm.msinfo
    echo "" >&2

    # Generate phrase model
    echo "* Generating phrase model... " >&2
    $bindir/thot_gen_phr_model -g ${scorpus} -m ${m_val} \
        -o ${outp} ${pml_opt} || exit 1
    echo "" >&2

    # Generate additional phrase model parameter files
    echo "* Generating additional phrase model parameter files... " >&2
    echo "${lambda_default_val} ${lambda_default_val}" > ${outp}.lambda
    echo ${sslen_default_val} > ${outp}.srcsegmlentable
    echo ${stopj_default_val} > ${outp}.trgcutstable
    echo ${tslen_default_val} > ${outp}.trgsegmlentable
    echo "" >&2
}

########
gen_final_alig_file()
{
    if [ ${g_given} -eq 1 ]; then
        # Set external alignment file
        echo "* Setting external word alignment file in GIZA format... " >&2
        cp ${gfile} ${outp}.A3.final
        echo "" >&2
    else
        if [ ${dict_given} -eq 1 ]; then
            # Generate exhaustive alignments so as to introduce sentence
            # pairs as phrase table entries
            echo "* Generating exhaustive word alignments... " >&2
            $bindir/thot_gen_exhaustive_giza_alig -s $scorpus -t $tcorpus > ${outp}.A3.final
            echo "" >&2
        else
            # Operate word alignments generated with the sw_models package
            echo "* Operating word alignments... " >&2
            $bindir/thot_pbs_alig_op -pr ${pr_val} -g ${outp}_swm.bestal ${ao_opt} ${outp}_invswm.bestal -o ${outp} \
                                     ${qs_opt} "${qs_par}" -sdir $sdir -T $tdir ${debug_opt} || exit 1
            echo "" >&2
        fi
    fi
}

########
standard_train()
{
    # Generate direct and inverse single word models using sw_models package

    # Generate direct single word model
    echo "* Generating source-to-target single word alignment model... " >&2
    echo "Warning: this process may be slow with large corpora, see Troubleshooting section in Thot manual for possible workarounds" >&2
    ${bindir}/thot_pbs_gen_batch_sw_model -pr ${pr_val} -s $scorpus -t $tcorpus -n ${niters} ${lf_opt} ${af_opt} ${np_opt} \
        -cpr ${cpr_val} ${shuff_opt} -o ${outp}_swm ${qs_opt} "${qs_par}" -sdir $sdir -tdir $tdir ${debug_opt} || exit 1
    echo "" >&2

    # Generate best alignments for direct model
    echo "* Generating best alignment for source-to-target model... " >&2
    ${bindir}/thot_pbs_gen_best_sw_alig -pr ${pr_val} -sw ${outp}_swm -s $scorpus -t $tcorpus \
        ${shuff_opt} -o ${outp}_swm ${qs_opt} "${qs_par}" -sdir $sdir -tdir $tdir ${debug_opt} || exit 1
    echo "" >&2

    # Generate inverse single word model
    echo "* Generating target-to-source single word alignment model... " >&2
    echo "Warning: this process may be slow with large corpora, see Troubleshooting section in Thot manual for possible workarounds" >&2
    ${bindir}/thot_pbs_gen_batch_sw_model -pr ${pr_val} -s $tcorpus -t $scorpus -n ${niters} ${lf_opt} ${af_opt} ${np_opt} \
        -cpr ${cpr_val} ${shuff_opt} -o ${outp}_invswm ${qs_opt} "${qs_par}" -sdir $sdir -tdir $tdir ${debug_opt} || exit 1
    echo "" >&2

    # Generate best alignments for inverse model
    echo "* Generating best alignment for target-to-source model... " >&2
    ${bindir}/thot_pbs_gen_best_sw_alig -pr ${pr_val} -sw ${outp}_invswm -s $tcorpus -t $scorpus \
        ${shuff_opt} -o ${outp}_invswm ${qs_opt} "${qs_par}" -sdir $sdir -tdir $tdir ${debug_opt} || exit 1
    echo "" >&2

    # Generate final alignment file
    gen_final_alig_file

    # Generate phrase model
    echo "* Generating phrase model... " >&2
    $bindir/thot_pbs_gen_phr_model -pr ${pr_val} -g ${outp}.A3.final -m ${m_val} \
        -o ${outp} ${pml_opt} ${qs_opt} "${qs_par}" -sdir $sdir -T $tdir ${debug_opt} || exit 1
    echo "" >&2

    # Constrain number of translation options
    echo "* Constraining number of translation options... " >&2
    $bindir/thot_pbs_get_nbest_for_trg -t ${outp}.ttable -n ${to_val} \
        -p -T $tdir -o ${outp}_restrict_trans_opt.ttable || exit 1
    rm ${outp}.ttable || exit 1
    mv ${outp}_restrict_trans_opt.ttable ${outp}.ttable || exit 1
    echo "" >&2

    # Generate additional phrase model parameter files
    echo "* Generating additional phrase model parameter files... " >&2
    echo "${lambda_default_val} ${lambda_default_val}" > ${outp}.lambda
    echo ${sslen_default_val} > ${outp}.srcsegmlentable
    echo ${stopj_default_val} > ${outp}.trgcutstable
    echo ${tslen_default_val} > ${outp}.trgsegmlentable
    echo "" >&2
}

########
if [ $# -lt 1 ]; then
    echo "Usage: thot_pbs_gen_batch_phr_model -pr <int>"
    echo "                                    -s <string> -t <string> -o <string>"
    echo "                                    [-g <string>] [-nit <int>] [-np <float>]"
    echo "                                    [-lf <float>] [-af <float>] [-cpr <float>]"
    echo "                                    [-m <int>] [-ao <string>] [-to <int>]"
    echo "                                    [-dict] [-unk] [-nsh] [-qs <string>]"
    echo "                                    [-T <string>] [-sdir <string>] [-debug]"
    echo ""
    echo "-pr <int>               Number of processors"
    echo "-s <string>             File with source sentences (give absolute path when"
    echo "                        using pbs clusters)"
    echo "-t <string>             File with target sentences (give absolute path when"
    echo "                        using pbs clusters)"
    echo "-o <string>             Prefix of the output files (give absolute path when"
    echo "                        using pbs clusters)"
    echo "-g <string>             Use external alignment file in GIZA format for phrase"
    echo "                        extraction. Thot's native alignment file is not used" 
    echo "-nit <int>              Number of iterations of the EM algorithm executed by"
    echo "                        the thot_gen_sw_model tool (5 by default)"
    echo "-lf <float>             Lexical smoothing interpolation factor for single-word"
    echo "                        models"
    echo "-af <float>             Alignment smoothing interpolation factor for"
    echo "                        single-word models"
    echo "-np <float>             Probability assigned to the alignment with the NULL"
    echo "                        word for single-word models"
    echo "-cpr <float>            Pruning parameter used during the estimation of single"
    echo "                        word alignment models (0.00001 by default)"
    echo "-m <int>                Maximum target phrase length during phrase model"
    echo "                        estimation (7 by default)"
    echo "-ao <string>            Operation between alignments to be executed"
    echo "                        (and|or|sum|sym1|sym2|grd)."
    echo "-to <int>               Maximum number of translation options for each target"
    echo "                        phrase that are considered during a translation process"
    echo "                        (20 by default)"
    echo "-dict                   Input data is considered as a dictionary, so the"
    echo "                        sentence pairs are introduced as phrase table entries"
    echo "-unk                    Introduce special unknown word symbol during"
    echo "                        estimation."
    echo "-nsh                    Do not shuffle load during training (shuffling is"
    echo "                        important to achieve load balancing)"
    echo "-qs <string>            Specific options to be given to the qsub"
    echo "                        command (example: -qs \"-l pmem=1gb\")"
    echo "-T <string>             Use <string> for temporaries instead of /tmp"
    echo "                        NOTES:"
    echo "                         a) give absolute paths when using pbs clusters."
    echo "                         b) ensure there is enough disk space in the partition"
    echo "-sdir <string>          Absolute path of a directory common to all"
    echo "                        processors. If not given, \$HOME will be used."
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
    nit_given=0
    g_given=0
    lf_given=0
    af_given=0
    np_given=0
    cpr_given=0
    cpr_val=0.00001
    niters=5
    m_val=7
    ao_given=0
    ao_opt="-sym1"
    to_given=0
    to_val=20
    qs_given=0
    dict_given=0
    unk_given=0
    nsh_given=0
    shuff_opt="-shu"
    tdir_given=0
    tdir=/tmp
    sdir_given=0
    sdir=$HOME
    debug=0
    lambda_default_val=0.01
    sslen_default_val="Uniform"
    tslen_default_val="Geometric"
    stopj_default_val=0.999

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
            "-nit") shift
                if [ $# -ne 0 ]; then
                    niters=$1
                    nit_given=1
                fi
                ;;
            "-g") shift
                if [ $# -ne 0 ]; then
                    gfile=$1
                    g_given=1
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
                    cpr_val=$1
                    cpr_given=1
                fi
                ;;
            "-m") shift
                if [ $# -ne 0 ]; then
                    m_val=$1
                    m_given=1
                fi
                ;;
            "-ao") shift
                if [ $# -ne 0 ]; then
                    ao_opt="-$1"
                    ao_given=1
                fi
                ;;
            "-to") shift
                if [ $# -ne 0 ]; then
                    to_val=$1
                    to_given=1
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
            "-dict") dict_given=1
                ;;
            "-unk") unk_given=1
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
        echo "Error: number of processors not given" >&2
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

    if [ ${g_given} -eq 1 -a ! -f ${gfile} ]; then        
        echo "Error! file ${gfile} does not exist" >&2
        exit 1            
    fi

    # Verify if unknown words are to be added to training corpus
    if [ ${unk_given} -eq 1 ]; then
        treat_unk_opt
    fi

    # Obtain number of lines for corpus files
    srcnl=`$WC -l $scorpus | $AWK '{printf"%s",$1}'`
    trgnl=`$WC -l $tcorpus | $AWK '{printf"%s",$1}'`

    # Train models
    if [ $srcnl -eq 0 -a $trgnl -eq 0 ]; then
        empty_train
    else
        standard_train
    fi

    exit 0
fi
