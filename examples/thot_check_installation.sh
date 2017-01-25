# *- bash -*

# Check the Thot package

# Create directory for temporary files
echo "**** Creating directory for temporary files..."
echo ""
tmpdir=`mktemp -d $HOME/thot_installcheck_XXXXXX`
# trap "rm -rf $tmpdir 2>/dev/null" EXIT
echo "Temporary files will be stored in ${tmpdir}"
echo ""

# Create directory for debugging information
debugdir=$tmpdir/debug
mkdir $debugdir

# Print warning about possible resource requirements when the software
# is installed on computer clusters
if [ ${QSUB_WORKS} = "yes" ]; then

    qs_opt="-qs"

    echo "------------------------------------------------------------------------" >&2
    echo "NOTE: package was installed on a PBS cluster, you may need to reserve" >&2
    echo "cluster resources in order to successfully execute the Thot tools." >&2
    echo "" >&2
    echo "To reserve resources for this checking, export the \"qs_par\" variable." >&2
    if [ -z "${qs_par}" ]; then
        echo "Warning: the \"qs_par\" variable is empty." >&2
        echo "Sample initialisations for the \"qs_par\":" >&2
        echo "" >&2
        echo "export qs_par=\"-l pmem=1gb\"" >&2
        echo "export qs_par=\"-l h_vmem=4G,h_rt=10:00:00\"" >&2
    else
        echo "Current value of \"qs_par\": \"${qs_par}\"" >&2
    fi
    echo "" >&2
    echo "Please consider the use of the -qs option with the different tools" >&2
    echo "provided by Thot." >&2
    echo "------------------------------------------------------------------------" >&2
    echo "" >&2
    sleep 5

else
    qs_opt=""
    qs_par=""
fi

# Basic installation checking
echo "**** Check static and dynamic tool configuration (thot_get_ll_weights)..."
echo ""
${bindir}/thot_get_ll_weights -v
if test $? -eq 0 ; then
    echo "... Done"
else
    echo "================================================"
    echo " Test failed!"
    echo " See additional information in ${tmpdir}"
    echo " Please report to "${bugreport}
    echo "================================================"
    echo ""
    echo "IMPORTANT NOTE: if you got an error message telling that 'libthot.so' is not"
    echo "found, then you need to execute the 'ldconfig' command as root immediately"
    echo "after 'make install'. This is a generic software installation problem caused"
    echo "by autotools that cannot be solved in Thot in a portable manner."
    echo ""
    exit 1
fi

echo "" 

# Check thot_lm_train
echo "**** Checking thot_lm_train..."
echo ""
${bindir}/thot_lm_train -c $datadir/toy_corpus/en_tok_lc.train -o $tmpdir/lm -n 4 -unk \
     -tdir $debugdir -sdir ${debugdir} ${qs_opt} "${qs_par}" -debug
if test $? -eq 0 ; then
    echo "... Done"
else
    echo "================================================"
    echo " Test failed!"
    echo " See additional information in ${tmpdir}"
    echo " Please report to "${bugreport}
    echo "================================================"
    exit 1
fi

echo ""

# Define corpus variables
scorpus_train=$datadir/toy_corpus/sp_tok_lc.train
scorpus_dev=$datadir/toy_corpus/sp_tok_lc.dev
scorpus_test=$datadir/toy_corpus/sp_tok_lc.test
tcorpus_train=$datadir/toy_corpus/en_tok_lc.train
tcorpus_dev=$datadir/toy_corpus/en_tok_lc.dev
tcorpus_test=$datadir/toy_corpus/en_tok_lc.test

# Show corpus variables
echo "**** Source partition files: ${scorpus_train} ${scorpus_dev} ${scorpus_test}"
echo ""
echo "**** Target partition files: ${tcorpus_train} ${tcorpus_dev} ${tcorpus_test}" 
echo ""

# Check thot_tm_train
echo "**** Checking thot_tm_train..."
echo ""
${bindir}/thot_tm_train -s ${scorpus_train} -t ${tcorpus_train} \
    -o $tmpdir/tm -nit 5 -tdir $debugdir -sdir ${debugdir} ${qs_opt} "${qs_par}" -debug
if test $? -eq 0 ; then
    echo "... Done"
else
    echo "================================================"
    echo " Test failed!"
    echo " See additional information in ${tmpdir}"
    echo " Please report to "${bugreport}
    echo "================================================"
    exit 1
fi

echo ""

# Checking thot_gen_cfg_file
echo "**** Checking thot_gen_cfg_file..."
echo ""
${bindir}/thot_gen_cfg_file $tmpdir/lm/lm_desc $tmpdir/tm/tm_desc > $tmpdir/server.cfg 
if test $? -eq 0 ; then
    echo "... Done"
else
    echo "================================================"
    echo " Test failed!"
    echo " See additional information in ${tmpdir}"
    echo " Please report to "${bugreport}
    echo "================================================"
    exit 1
fi

echo ""

# Check thot_smt_tune
echo "**** Checking thot_smt_tune..."
echo ""
${bindir}/thot_smt_tune -c $tmpdir/server.cfg -s ${scorpus_dev} -t ${tcorpus_dev} \
    -o $tmpdir/smt_tune  -tdir $debugdir -sdir ${debugdir} ${qs_opt} "${qs_par}" -debug
if test $? -eq 0 ; then
    echo "... Done"
else
    echo "================================================"
    echo " Test failed!"
    echo " See additional information in ${tmpdir}"
    echo " Please report to "${bugreport}
    echo "================================================"
    exit 1
fi

echo ""

# Check thot_prepare_sys_for_test
echo "**** Checking thot_prepare_sys_for_test..."
echo ""
${bindir}/thot_prepare_sys_for_test -c $tmpdir/smt_tune/tuned_for_dev.cfg -t ${scorpus_test} \
    ${qs_opt} "${qs_par}" -o $tmpdir/systest -tdir $debugdir -sdir ${debugdir}
if test $? -eq 0 ; then
    echo "... Done"
else
    echo "================================================"
    echo " Test failed!"
    echo " See additional information in ${tmpdir}"
    echo " Please report to "${bugreport}
    echo "================================================"
    exit 1
fi

echo ""

# Check thot_decoder
echo "**** Checking thot_decoder..."
echo ""
${bindir}/thot_decoder -c $tmpdir/systest/test_specific.cfg -t ${scorpus_test} \
    -o $tmpdir/thot_decoder_out -sdir $debugdir ${qs_opt} "${qs_par}" -debug
if test $? -eq 0 ; then
    echo "... Done"
else
    echo "================================================"
    echo " Test failed!"
    echo " See additional information in ${tmpdir}"
    echo " Please report to "${bugreport}
    echo "================================================"
    exit 1
fi

echo ""

# Check thot_calc_bleu
echo "**** Checking thot_calc_bleu..."
echo ""
${bindir}/thot_calc_bleu -r ${tcorpus_test} -t $tmpdir/thot_decoder_out
if test $? -eq 0 ; then
    echo "... Done"
else
    echo "================================================"
    echo " Test failed!"
    echo " See additional information in ${tmpdir}"
    echo " Please report to "${bugreport}
    echo "================================================"
    exit 1
fi

echo ""

# Check thot_scorer
echo "**** Checking thot_scorer..."
echo ""
${bindir}/thot_scorer -r ${tcorpus_test} -t $tmpdir/thot_decoder_out
if test $? -eq 0 ; then
    echo "... Done"
else
    echo "================================================"
    echo " Test failed!"
    echo " See additional information in ${tmpdir}"
    echo " Please report to "${bugreport}
    echo "================================================"
    exit 1
fi

echo ""

# Remove directories for temporaries
echo "*** Remove directories used to store temporary files..."
echo ""
rm -rf $tmpdir

echo ""
echo "================"
echo "All tests passed"
echo "================"
