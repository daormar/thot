# *- bash -*

# Check the Thot package

# Create directory for temporary files
echo "**** Creating directory for temporary files..."
echo ""
tmpdir=`mktemp -d`
# trap "rm -rf $tmpdir 2>/dev/null" EXIT
echo "Temporary files will be stored in ${tmpdir}"
echo ""

# Create directory for debugging information
debugdir=$tmpdir/debug
mkdir $debugdir

# Check thot_lm_train
echo "**** Checking thot_lm_train..."
echo ""
${bindir}/thot_lm_train -c $datadir/toy_corpus/en.train -o $tmpdir/lm -n 4 -unk \
     -tdir $debugdir -sdir $debugdir -debug
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

# Check thot_tm_train
echo "**** Checking thot_tm_train..."
echo ""
${bindir}/thot_tm_train -s $datadir/toy_corpus/sp.train -t $datadir/toy_corpus/en.train \
    -o $tmpdir/tm -n 5 -tdir $debugdir -sdir $debugdir -debug
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
${bindir}/thot_smt_tune -c $tmpdir/server.cfg -s $datadir/toy_corpus/sp.dev -t $datadir/toy_corpus/en.dev \
    -o $tmpdir/tune  -tdir $debugdir -sdir $debugdir -debug
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
${bindir}/thot_prepare_sys_for_test -c $tmpdir/tune/tuned_for_dev.cfg -t $datadir/toy_corpus/sp.test -o $tmpdir/systest
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
${bindir}/thot_decoder -c $tmpdir/systest/test_specific.cfg -t $datadir/toy_corpus/sp.test \
    -o $tmpdir/thot_decoder_out -sdir $debugdir -debug
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
${bindir}/thot_calc_bleu -r $datadir/toy_corpus/en.test -t $tmpdir/thot_decoder_out
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

# Remove directory for temporaries
echo "*** Remove directory for temporary files..."
echo ""
rm -rf $tmpdir

echo ""
echo "================"
echo "All tests passed"
echo "================"
