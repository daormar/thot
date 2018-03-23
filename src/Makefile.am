# general settings

LPTHREAD=@LPTHREAD@
LGMP=@LGMP@
ZLIB=@ZLIB@
BZ2LIB=@BZ2LIB@
LZMALIB=@LZMALIB@
LDB_CXX=@LDB_CXX@
LEVELDB_LIB=@LEVELDB_LIB@
CPPUNIT=@CPPUNIT@

if HAVE_DB_CXX_LIB
if HAVE_DB_CXX_H
DB_CXX_PROGS=thot_ttable_to_fbdb
DB_CXX_LIBS=bdb_phrase_model_factory.la fast_bdb_phrase_model_factory.la
endif
endif

if HAVE_LEVELDB_LIB
LEVELDB_PROGS=thot_ngram_to_leveldb thot_ttable_to_leveldb	\
thot_lextable_to_leveldb
LEVELDB_LIBS=incr_jel_mer_leveldb_ngram_lm_factory.la	\
leveldb_phrase_model_factory.la				\
incr_leveldb_hmm_p0_alig_model_factory.la		\
leveldb_dict_feat__phrscoreinfo_factory.la
endif

AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS= nlp_common incr_models sw_models phrase_models smt_preproc	\
error_correction downhill_simplex stack_dec exper testing hat_trie	\
picojson

if CODE_TESTING
TESTING_PROGS= thot_test
endif

if CASMACAT_LIB_ENABLED
CASMACAT_LIB= libthot_casmacat.la
endif

if KENLM_LIB_ENABLED
KENLM_CXXFLAGS= -I$(KENLM_BUILD_DIR)/include -DKENLM_MAX_ORDER=6
KENLM_LIBADD= -L$(KENLM_BUILD_DIR)/lib -lkenlm -lkenlm_util $(BZ2LIB)	\
$(LZMALIB)
KENLM_LDADD= -L$(KENLM_BUILD_DIR)/lib -lkenlm -lkenlm_util $(BZ2LIB)	\
$(LZMALIB)
KENLM_LIB= kenlm_factory.la
endif

bin_PROGRAMS = thot_lm_perp thot_ilm_perp thot_lm_weight_upd		\
thot_calc_swm_lgprob thot_gen_sw_model thot_sort_bin_ilextable		\
thot_sort_bin_ihmmatable thot_sort_bin_iibm2atable			\
thot_merge_bin_ilextable thot_merge_bin_ihmmatable			\
thot_merge_bin_iibm2atable thot_gen_bin_lex_filter_info			\
thot_filter_bin_ilextable thot_prune_bin_ilextable thot_alig_op		\
thot_query_pm thot_gen_phr_model thot_wg_proc thot_dhs_step_by_step_min	\
thot_ms_dec thot_ms_alig thot_li_weight_upd thot_ll_weight_upd_nblist	\
thot_client thot_server thot_get_srcsents_from_metadata			\
thot_check_constraints thot_scorer thot_calc_bleu $(DB_CXX_PROGS)	\
$(LEVELDB_PROGS) $(TESTING_PROGS)

lib_LTLIBRARIES = libthot.la word_penalty_model_factory.la		\
incr_jel_mer_ngram_lm_factory.la					\
smoothed_incr_ibm2_alig_model_factory.la				\
incr_hmm_p0_alig_model_factory.la incr_phrase_model_factory.la		\
wba_incr_phrase_model_factory.la pfsm_ecm_for_wg_factory.la		\
non_pb_ec_model_for_nb_ucat_factory.la					\
wg_processor_for_anlp__pfsm_factory.la mira_bleu_factory.la		\
mira_wer_factory.la mira_gtm_factory.la mira_chrf_factory.la		\
kb_mira_ll_wu_factory.la dict_feat__phrscoreinfo_factory.la		\
multi_stack_decoder_rec__swli_factory.la				\
wg_uncoupled_assisted_trans__swli_factory.la				\
multi_stack_decoder_rec__pbtm_factory.la				\
wg_uncoupled_assisted_trans__pbtm_factory.la				\
translation_metadata__phrscoreinfo_factory.la				\
json_translation_metadata__phrscoreinfo_factory.la $(CASMACAT_LIB)	\
$(KENLM_LIB) $(DB_CXX_LIBS) $(LEVELDB_LIBS)

# flags
AM_CXXFLAGS= -W -Wno-deprecated -I$(srcdir)/nlp_common			   \
-I$(srcdir)/incr_models -I$(srcdir)/sw_models -I$(srcdir)/phrase_models	   \
-I$(srcdir)/smt_preproc -I$(srcdir)/error_correction			   \
-I$(srcdir)/downhill_simplex -I$(srcdir)/stack_dec -I$(srcdir)/hat_trie	   \
-I$(srcdir)/picojson							   \
-DTHOT_MASTER_INI_PATH=\"$(datadir)/$(PACKAGE_NAME)/ini_files/master.ini\" \
-DTHOT_LIBDIR=\"$(libdir)\"						   \
-I$(CASMACAT_THOT_SERVER_LIB_HOME)/src/include $(KENLM_CXXFLAGS)

AM_CFLAGS= $(AM_CXXFLAGS)

LDADD = $(KENLM_LDADD) $(LGMP) $(ZLIB) $(LDB_CXX) $(LEVELDB_LIB)	\
$(CPPUNIT) $(LPTHREAD) -ldl

# sources
common_src_h= thot_config.h

nlp_common_h= nlp_common/WordIndex.h nlp_common/WordAligMatrix.h	\
nlp_common/uiPairHashF.h nlp_common/uiHashF.h nlp_common/TrieVecs.h	\
nlp_common/Trie.h nlp_common/BidTrie.h nlp_common/StrProcUtils.h	\
nlp_common/ModelDescriptorUtils.h nlp_common/StatModelDefs.h		\
nlp_common/SingleWordVocab.h nlp_common/Score.h nlp_common/Prob.h	\
nlp_common/printAligFuncs.h nlp_common/PositionIndex.h			\
nlp_common/OrderedVector.h nlp_common/options.h				\
nlp_common/NbestTransTable.h nlp_common/NbestTableNode.h		\
nlp_common/mem_alloc_utils.h nlp_common/MathFuncs.h			\
nlp_common/MathDefs.h nlp_common/lt_op_vec.h nlp_common/LogCount.h	\
nlp_common/LM_Defs.h nlp_common/SmtDefs.h nlp_common/ins_op_pair.h	\
nlp_common/getline.h nlp_common/getdelim.h nlp_common/ErrorDefs.h	\
nlp_common/ctimer.h nlp_common/Count.h nlp_common/Bitset.h		\
nlp_common/BasicSocketUtils.h nlp_common/BaseNgramLM.h			\
nlp_common/BaseIncrNgramLM.h nlp_common/AwkInputStream.h		\
nlp_common/DynClassFileHandler.h nlp_common/SimpleDynClassLoader.h	\
nlp_common/ThreadSafePrint.h nlp_common/StdCerrThreadSafeTidPrint.h	\
nlp_common/StdCerrThreadSafePrint.h
nlp_common_defs= nlp_common/WordAligMatrix.cc			\
nlp_common/StrProcUtils.cc nlp_common/ModelDescriptorUtils.cc	\
nlp_common/SingleWordVocab.cc nlp_common/Prob.cc		\
nlp_common/printAligFuncs.cc nlp_common/options.cc		\
nlp_common/mem_alloc_utils.cc nlp_common/MathFuncs.cc		\
nlp_common/getline.c nlp_common/getdelim.c nlp_common/ctimer.c	\
nlp_common/BasicSocketUtils.cc nlp_common/AwkInputStream.cc	\
nlp_common/DynClassFileHandler.cc

incr_models_h= incr_models/vecx_x_incr_enc.h				\
incr_models/vecx_x_incr_ecpm.h incr_models/vecx_x_incr_cptable.h	\
incr_models/lm_ienc.h incr_models/IncrNgramLM.h				\
incr_models/_incrNgramLM.h incr_models/IncrJelMerNgramLM.h		\
incr_models/_incrJelMerNgramLM.h incr_models/IncrEncoder.h		\
incr_models/_incrEncCondProbModel.h incr_models/IncrCondProbTable.h	\
incr_models/im_pair.h incr_models/BaseIncrEncoder.h			\
incr_models/BaseIncrEncCondProbModel.h					\
incr_models/BaseIncrCondProbTable.h incr_models/BaseIncrCondProbModel.h	\
incr_models/BaseWordPenaltyModel.h incr_models/WordPenaltyModel.h	\
incr_models/WordPredictor.h
incr_models_defs= incr_models/lm_ienc.cc incr_models/IncrNgramLM.cc	\
incr_models/IncrJelMerNgramLM.cc incr_models/WordPenaltyModel.cc	\
incr_models/WordPredictor.cc

if KENLM_LIB_ENABLED
kenlm_h= nlp_common/KenLm.h
kenlm_defs= nlp_common/KenLm.cc
else
kenlm_h=
kenlm_defs=
endif

if HAVE_LEVELDB_LIB
leveldb_lm_h= incr_models/LevelDbNgramTable.h	\
incr_models/IncrJelMerLevelDbNgramLM.h
leveldb_lm_defs= incr_models/LevelDbNgramTable.cc	\
incr_models/IncrJelMerLevelDbNgramLM.cc
endif

sw_models_h= sw_models/WeightedIncrNormSlm.h sw_models/SwDefs.h		\
sw_models/_swAligModel.h sw_models/SmoothedIncrIbm2AligModel.h		\
sw_models/SmoothedIncrIbm1AligModel.h sw_models/SentPairCont.h		\
sw_models/_sentLengthModel.h sw_models/LightSentenceHandler.h		\
sw_models/LexAuxVar.h sw_models/_incrSwAligModel.h			\
sw_models/_incrLexTable.h sw_models/_incrHmmAligModel.h			\
sw_models/IncrLexTable.h sw_models/IncrIbm2AligTable.h			\
sw_models/IncrIbm2AligModel.h sw_models/IncrIbm1AligModel.h		\
sw_models/_incrHmmP0AligModel.h sw_models/IncrHmmP0AligModel.h		\
sw_models/IncrHmmAligTable.h sw_models/IncrHmmAligModel.h		\
sw_models/HmmAligInfo.h sw_models/CachedHmmAligLgProb.h			\
sw_models/CachedHmmAligLgProb.cc sw_models/DoubleMatrix.h		\
sw_models/BestLgProbForTrgWord.h sw_models/BaseSwAligModel.h		\
sw_models/BaseStepwiseAligModel.h sw_models/BaseSentLengthModel.h	\
sw_models/BaseSentenceHandler.h sw_models/aSourceHmm.h			\
sw_models/aSourceHashF.h sw_models/aSource.h				\
sw_models/ashPidxPairHashF.h sw_models/anjm1ip_anjiMatrix.h		\
sw_models/anjiMatrix.h
sw_models_defs= sw_models/WeightedIncrNormSlm.cc			\
sw_models/SmoothedIncrIbm2AligModel.cc					\
sw_models/SmoothedIncrIbm1AligModel.cc sw_models/_sentLengthModel.cc	\
sw_models/_incrHmmAligModel.cc sw_models/_incrHmmP0AligModel.cc		\
sw_models/LightSentenceHandler.cc sw_models/IncrLexTable.cc		\
sw_models/IncrIbm2AligTable.cc sw_models/IncrIbm2AligModel.cc		\
sw_models/IncrIbm1AligModel.cc sw_models/IncrHmmP0AligModel.cc		\
sw_models/IncrHmmAligTable.cc sw_models/IncrHmmAligModel.cc		\
sw_models/DoubleMatrix.cc sw_models/aSourceHmm.cc sw_models/aSource.cc	\
sw_models/anjm1ip_anjiMatrix.cc sw_models/anjiMatrix.cc

if HAVE_LEVELDB_LIB
leveldb_sw_h= sw_models/IncrLexLevelDbTable.h	\
sw_models/IncrLevelDbHmmAligModel.h		\
sw_models/IncrLevelDbHmmP0AligModel.h
leveldb_sw_defs= sw_models/IncrLexLevelDbTable.cc	\
sw_models/IncrLevelDbHmmAligModel.cc			\
sw_models/IncrLevelDbHmmP0AligModel.cc
endif

if CASMACAT_LIB_ENABLED
casmacat_aligner_h= sw_models/ThotIbm2AlignerFactory.h	\
sw_models/ThotIbm2Aligner.h sw_models/ThotHmmAligner.h	\
sw_models/ThotHmmAlignerFactory.h
casmacat_aligner_defs= sw_models/ThotIbm2AlignerFactory.cc	\
sw_models/ThotIbm2Aligner.cc sw_models/ThotHmmAligner.cc	\
sw_models/ThotHmmAlignerFactory.cc

casmacat_confid_h= sw_models/ThotIbmMaxConfid.h	\
sw_models/ThotIbmMaxConfidFactory.h
casmacat_confid_defs= sw_models/ThotIbmMaxConfid.cc	\
sw_models/ThotIbmMaxConfidFactory.cc
else
casmacat_alig_h=
casmacat_alig_defs=

casmacat_confid_h=
casmacat_confid_defs=
endif

phrase_models_h= phrase_models/WbaIncrPhraseModel.h			\
phrase_models/_wbaIncrPhraseModel.h					\
phrase_models/VecUnsignedIntSortCriterion.h				\
phrase_models/TrgSegmLenTable.h phrase_models/TrgCutsTable.h		\
phrase_models/SrfNodeKey.h phrase_models/SrfNodeInfoMap.h		\
phrase_models/SrfNodeInfo.h phrase_models/SrfBisegm.h			\
phrase_models/SrcSegmLenTable.h phrase_models/SentSegmentation.h	\
phrase_models/SegLenTable.h phrase_models/PhraseTransTableNodeData.h	\
phrase_models/StlPhraseTable.h phrase_models/PhraseSortCriterion.h	\
phrase_models/PhrasePairInfo.h phrase_models/PhrasePair.h		\
phrase_models/PhraseId.h phrase_models/PhraseExtractParameters.h	\
phrase_models/PhraseExtractionTable.h					\
phrase_models/PhraseExtractionCell.h phrase_models/PhraseDefs.h		\
phrase_models/_incrPhraseModel.h phrase_models/IncrPhraseModel.h	\
phrase_models/CellID.h phrase_models/CellAlignment.h			\
phrase_models/BpSetInfo.h phrase_models/BpSet.h				\
phrase_models/BasePhraseTable.h phrase_models/BasePhraseModel.h		\
phrase_models/BaseIncrPhraseModel.h					\
phrase_models/BaseCountPhraseModel.h phrase_models/AlignmentExtractor.h	\
phrase_models/AlignmentContainer.h phrase_models/AligInfo.h		\
phrase_models/BasePhrasePairFilter.h					\
phrase_models/CategPhrasePairFilter.h					\
phrase_models/StrictCategPhrasePairFilter.h				\
phrase_models/PhraseExtractUtils.h
phrase_models_defs= phrase_models/WbaIncrPhraseModel.cc			\
phrase_models/_wbaIncrPhraseModel.cc phrase_models/TrgSegmLenTable.cc	\
phrase_models/TrgCutsTable.cc phrase_models/SrfNodeKey.cc		\
phrase_models/SrfNodeInfoMap.cc phrase_models/SrcSegmLenTable.cc	\
phrase_models/SegLenTable.cc phrase_models/StlPhraseTable.cc		\
phrase_models/PhraseExtractionTable.cc					\
phrase_models/_incrPhraseModel.cc phrase_models/IncrPhraseModel.cc	\
phrase_models/BpSet.cc phrase_models/BasePhraseModel.cc			\
phrase_models/BaseIncrPhraseModel.cc					\
phrase_models/AlignmentExtractor.cc phrase_models/AlignmentContainer.cc	\
phrase_models/CategPhrasePairFilter.cc					\
phrase_models/StrictCategPhrasePairFilter.cc				\
phrase_models/PhraseExtractUtils.cc

if HAVE_DB_CXX_LIB
if HAVE_DB_CXX_H
bdb_pm_h= phrase_models/BdbPhraseTable.h phrase_models/BdbPhraseModel.h	\
phrase_models/FastBdbPhraseTable.h phrase_models/FastBdbPhraseModel.h
bdb_pm_defs= phrase_models/BdbPhraseTable.cc				\
phrase_models/BdbPhraseModel.cc phrase_models/FastBdbPhraseTable.cc	\
phrase_models/FastBdbPhraseModel.cc
endif
endif

if HAVE_LEVELDB_LIB
leveldb_pm_h=phrase_models/LevelDbPhraseTable.h	\
phrase_models/LevelDbPhraseModel.h
leveldb_pm_defs= phrase_models/LevelDbPhraseTable.cc	\
phrase_models/LevelDbPhraseModel.cc
endif

if HAVE_CXX11_ENABLED
hattrie_pm_h=phrase_models/HatTriePhraseTable.h
hattrie_pm_defs=phrase_models/HatTriePhraseTable.cc
endif

smt_preproc_h= smt_preproc/SmtPreprocDefs.h	\
smt_preproc/BasePrePosProcessor.h
smt_preproc_defs=

error_correction_h= error_correction/WordGraphStateData.h		\
error_correction/WordGraph.h error_correction/WordGraphArcId.h		\
error_correction/WordGraphArc.h error_correction/WordAndCharLevelOps.h	\
error_correction/WgProcessorForAnlp.h error_correction/WgHandler.h	\
error_correction/RejectedWordsSet.h error_correction/PrefAlignInfo.h	\
error_correction/PfsmEcm.h error_correction/PfsmEcmForWg.h		\
error_correction/PfsmEcmForWgEsi.h					\
error_correction/NonPbEcModelForNbUcat.h				\
error_correction/NbSearchStack.h error_correction/NbSearchHyp.h		\
error_correction/NbSearchHighLevelHyp.h					\
error_correction/NbestCorrections.h error_correction/HypStateIndex.h	\
error_correction/_editDist.h error_correction/EditDistForVecString.h	\
error_correction/EditDistForVec.h error_correction/EditDistForStr.h	\
error_correction/_editDistBasedEcm.h					\
error_correction/BaseWgProcessorForAnlp.h				\
error_correction/BaseErrorCorrectionModel.h				\
error_correction/BaseEditDist.h error_correction/BaseEcModelForNbUcat.h	\
error_correction/BaseEcmForWg.h
error_correction_defs= error_correction/WordGraph.cc			\
error_correction/WgHandler.cc error_correction/PfsmEcmForWg.cc		\
error_correction/PfsmEcm.cc error_correction/NonPbEcModelForNbUcat.cc	\
error_correction/NbSearchStack.cc					\
error_correction/EditDistForVecString.cc				\
error_correction/EditDistForStr.cc					\
error_correction/_editDistBasedEcm.cc					\
error_correction/BaseErrorCorrectionModel.cc

downhill_simplex_h= downhill_simplex/step_by_step_dhs.h
downhill_simplex_defs= downhill_simplex/step_by_step_dhs.c

stack_dec_h= stack_dec/LM_State.h stack_dec/PpInfo.h			\
stack_dec/SmtModel.h stack_dec/SmtModelLegacy.h				\
stack_dec/DynClassFactoryHandler.h stack_dec/WgUncoupledAssistedTrans.h	\
stack_dec/ThotDecoderUserPars.h stack_dec/ThotDecoderState.h		\
stack_dec/ThotDecoderPerUserVars.h stack_dec/ThotDecoder.h		\
stack_dec/ThotDecoderCommonVars.h stack_dec/ThotDecoderClient.h		\
stack_dec/SwModelPars.h stack_dec/_stack_decoder_statistics.h		\
stack_dec/_stackDecoderRec.h stack_dec/_stackDecoder.h			\
stack_dec/SourceSegmentation.h stack_dec/BaseTranslationMetadata.h	\
stack_dec/TranslationMetadata.h stack_dec/JsonTranslationMetadata.h	\
stack_dec/SmtStack.h stack_dec/_smtStack.h stack_dec/SmtMultiStackRec.h	\
stack_dec/_smtMultiStack.h stack_dec/WeightUpdateUtils.h		\
stack_dec/BaseLogLinWeightUpdater.h stack_dec/KbMiraLlWu.h		\
stack_dec/BaseScorer.h stack_dec/BaseMiraScorer.h stack_dec/MiraBleu.h	\
stack_dec/MiraWer.h stack_dec/MiraGtm.h stack_dec/MiraChrF.h		\
stack_dec/_smtModel.h stack_dec/ScoreCompDefs.h				\
stack_dec/_phrSwTransModel.h stack_dec/PhrScoreInfo.h			\
stack_dec/PhrNbestTransTableRefKey.h stack_dec/PhrNbestTransTableRef.h	\
stack_dec/PhrNbestTransTablePrefKey.h					\
stack_dec/PhrNbestTransTablePref.h stack_dec/PhrNbestTransTable.h	\
stack_dec/PhraseBasedTmHyp.h stack_dec/PhraseBasedTmHypRec.h		\
stack_dec/PhrLocalSwLiTmHypRec.h stack_dec/PhrLocalSwLiTm.h		\
stack_dec/PhrHypState.h stack_dec/PhrHypNumcovJumpsEqClassF.h		\
stack_dec/PhrHypNumcovJumps01EqClassF.h stack_dec/PhrHypEqClassF.h	\
stack_dec/PhrHypData.h stack_dec/PhrHypDataStr.h			\
stack_dec/PhrasePairCacheTable.h stack_dec/PbTransModelPars.h		\
stack_dec/PbTransModelInputVars.h stack_dec/NbestTransCacheData.h	\
stack_dec/PhraseModelPars.h stack_dec/PhraseModelInfo.h			\
stack_dec/PhraseModelsInfo.h stack_dec/SwModelsInfo.h			\
stack_dec/_phraseHypothesisRec.h stack_dec/_phraseHypothesis.h		\
stack_dec/PhraseCacheTable.h stack_dec/_phraseBasedTransModel.h		\
stack_dec/_pbTransModel.h stack_dec/PbTransModel.h			\
stack_dec/OnlineTrainingPars.h stack_dec/NgramCacheTable.h		\
stack_dec/_nbUncoupledAssistedTrans.h					\
stack_dec/multi_stack_decoder_rec.h stack_dec/WpModelInfo.h		\
stack_dec/LangModelPars.h stack_dec/LangModelInfo.h			\
stack_dec/LangModelsInfo.h stack_dec/FeaturesInfo.h			\
stack_dec/StdFeatureHandler.h stack_dec/CustomFeatureHandler.h		\
stack_dec/HypStateDict.h stack_dec/HypStateDictData.h			\
stack_dec/HypSortCriterion.h stack_dec/client_server_defs.h		\
stack_dec/CatDefs.h stack_dec/bleu.h stack_dec/chrf.h			\
stack_dec/BaseStackDecoder.h stack_dec/BaseSmtStack.h			\
stack_dec/BaseSmtMultiStack.h stack_dec/BasePbTransModelFeature.h	\
stack_dec/WordPenaltyFeat.h stack_dec/LangModelFeat.h			\
stack_dec/DirectPhraseModelFeat.h stack_dec/InversePhraseModelFeat.h	\
stack_dec/SrcPhraseLenFeat.h stack_dec/TrgPhraseLenFeat.h		\
stack_dec/SrcPosJumpFeat.h stack_dec/OnTheFlyDictFeat.h			\
stack_dec/DictFeat.h stack_dec/BaseSmtModel.h				\
stack_dec/BasePhraseHypothesisRec.h stack_dec/BasePhraseHypothesis.h	\
stack_dec/BasePbTransModelStats.h stack_dec/BasePbTransModel.h		\
stack_dec/BaseHypState.h stack_dec/BaseHypothesisRec.h			\
stack_dec/BaseHypothesis.h stack_dec/HypDebugData.h			\
stack_dec/BaseAssistedTrans.h stack_dec/_assistedTrans.h		\
stack_dec/SmtModelUtils.h
stack_dec_defs= stack_dec/DynClassFactoryHandler.cc			\
stack_dec/WeightUpdateUtils.cc stack_dec/KbMiraLlWu.cc			\
stack_dec/MiraBleu.cc stack_dec/MiraWer.cc stack_dec/MiraGtm.cc		\
stack_dec/MiraChrF.cc stack_dec/ThotDecoderClient.cc			\
stack_dec/ThotDecoder.cc stack_dec/StdFeatureHandler.cc			\
stack_dec/CustomFeatureHandler.cc stack_dec/WordPenaltyFeat.cc		\
stack_dec/LangModelFeat.cc stack_dec/DirectPhraseModelFeat.cc		\
stack_dec/InversePhraseModelFeat.cc stack_dec/SrcPhraseLenFeat.cc	\
stack_dec/TrgPhraseLenFeat.cc stack_dec/SrcPosJumpFeat.cc		\
stack_dec/OnTheFlyDictFeat.cc stack_dec/DictFeat.cc			\
stack_dec/PhrScoreInfo.cc stack_dec/PhrNbestTransTableRefKey.cc		\
stack_dec/PhrNbestTransTablePrefKey.cc stack_dec/PhrLocalSwLiTm.cc	\
stack_dec/PhrHypState.cc stack_dec/PhrHypNumcovJumpsEqClassF.cc		\
stack_dec/PhrHypNumcovJumps01EqClassF.cc stack_dec/PhrHypEqClassF.cc	\
stack_dec/bleu.cc stack_dec/chrf.cc stack_dec/BaseHypState.cc		\
stack_dec/SmtModelUtils.cc

if HAVE_LEVELDB_LIB
leveldb_stack_dec_h= stack_dec/LevelDbDict.h	\
stack_dec/LevelDbDictFeat.h
leveldb_stack_dec_defs= stack_dec/LevelDbDict.cc	\
stack_dec/LevelDbDictFeat.cc
endif

if CASMACAT_LIB_ENABLED
casmacat_engines_h= stack_dec/UserNameToUserIdMap.h		\
stack_dec/ThotMtEngine.h stack_dec/ThotMtFactoryInitPars.h	\
stack_dec/ThotMtFactory.h stack_dec/ThotImtEngine.h		\
stack_dec/ThotImtSession.h stack_dec/ThotImtFactoryInitPars.h	\
stack_dec/ThotImtFactory.h
casmacat_engines_defs= stack_dec/UserNameToUserIdMap.cc	\
stack_dec/ThotMtFactory.cc stack_dec/ThotMtEngine.cc	\
stack_dec/ThotImtSession.cc stack_dec/ThotImtFactory.cc	\
stack_dec/ThotImtEngine.cc
else
casmacat_engines_h=
casmacat_engines_defs=
endif

picojson_h=picojson/picojson.h

exper_h=
exper_defs=

testing_h= testing/KbMiraLlWuTest.h testing/MiraChrFTest.h		\
testing/TranslationMetadataTest.h testing/JsonTranslationMetadataTest.h	\
testing/_incrLexTableTest.h testing/_phraseTableTest.h			\
testing/IncrLexTableTest.h testing/StlPhraseTableTest.h

testing_defs= testing/KbMiraLlWuTest.cc testing/MiraChrFTest.cc		\
testing/TranslationMetadataTest.cc					\
testing/JsonTranslationMetadataTest.cc testing/_incrLexTableTest.cc	\
testing/_phraseTableTest.cc testing/IncrLexTableTest.cc			\
testing/StlPhraseTableTest.cc


if HAVE_LEVELDB_LIB
leveldb_pm_testing_h= testing/IncrLexLevelDbTableTest.h			\
testing/LevelDbNgramTableTest.h testing/LevelDbPhraseTableTest.h
leveldb_pm_testing_defs= testing/IncrLexLevelDbTableTest.cc		\
testing/LevelDbNgramTableTest.cc testing/LevelDbPhraseTableTest.cc
endif

if HAVE_CXX11_ENABLED
hattrie_pm_testing_h=testing/HatTriePhraseTableTest.h
hattrie_pm_testing_defs=testing/HatTriePhraseTableTest.cc
endif

thot_sources= $(common_src_h) $(nlp_common_h) $(nlp_common_defs)	\
$(incr_models_h) $(incr_models_defs) $(kenlm_h) $(kenlm_defs)		\
$(leveldb_lm_h) $(leveldb_lm_defs) $(sw_models_h) $(sw_models_defs)	\
$(phrase_models_h) $(phrase_models_defs) $(bdb_pm_h) $(bdb_pm_defs)	\
$(hattrie_pm_h) $(hattrie_pm_defs) $(leveldb_pm_h) $(leveldb_pm_defs)	\
$(leveldb_sw_h) $(leveldb_sw_defs) $(smt_preproc_h) $(smt_preproc_defs)	\
$(error_correction_h) $(error_correction_defs) $(downhill_simplex_h)	\
$(downhill_simplex_defs) $(stack_dec_h) $(stack_dec_defs)		\
$(leveldb_stack_dec_h) $(leveldb_stack_dec_defs) $(picojson_h)		\
$(exper_h) $(exper_defs)

# Class factories

##########
word_penalty_model_factory_h= 
word_penalty_model_factory_defs=		\
incr_models/WordPenaltyModelFactory.cc

##########
incr_jel_mer_ngram_lm_factory_h= 
incr_jel_mer_ngram_lm_factory_defs=		\
incr_models/IncrJelMerNgramLMFactory.cc

##########
incr_jel_mer_leveldb_ngram_lm_factory_h= 
incr_jel_mer_leveldb_ngram_lm_factory_defs=		\
incr_models/IncrJelMerLevelDbNgramLMFactory.cc

##########
kenlm_factory_h=
kenlm_factory_defs= nlp_common/KenLmFactory.cc

##########
smoothed_incr_ibm2_alig_model_factory_h= 
smoothed_incr_ibm2_alig_model_factory_defs=		\
sw_models/SmoothedIncrIbm2AligModelFactory.cc

##########
incr_hmm_p0_alig_model_factory_h= 
incr_hmm_p0_alig_model_factory_defs= sw_models/IncrHmmP0AligModelFactory.cc

##########
incr_leveldb_hmm_p0_alig_model_factory_h= 
incr_leveldb_hmm_p0_alig_model_factory_defs=		\
sw_models/IncrLevelDbHmmP0AligModelFactory.cc

##########
incr_phrase_model_factory_h= 
incr_phrase_model_factory_defs= phrase_models/IncrPhraseModelFactory.cc

##########
wba_incr_phrase_model_factory_h= 
wba_incr_phrase_model_factory_defs=		\
phrase_models/WbaIncrPhraseModelFactory.cc

##########
bdb_phrase_model_factory_h= 
bdb_phrase_model_factory_defs= phrase_models/BdbPhraseModelFactory.cc

##########
fast_bdb_phrase_model_factory_h= 
fast_bdb_phrase_model_factory_defs=		\
phrase_models/FastBdbPhraseModelFactory.cc

##########
leveldb_phrase_model_factory_h= 
leveldb_phrase_model_factory_defs= phrase_models/LevelDbPhraseModelFactory.cc

##########
pfsm_ecm_for_wg_factory_h= 
pfsm_ecm_for_wg_factory_defs=		\
error_correction/PfsmEcmForWgFactory.cc

##########
non_pb_ec_model_for_nb_ucat_factory_h= 
non_pb_ec_model_for_nb_ucat_factory_defs=		\
error_correction/NonPbEcModelForNbUcatFactory.cc

##########
wg_processor_for_anlp__pfsm_factory_h= 
wg_processor_for_anlp__pfsm_factory_defs=		\
error_correction/WgProcessorForAnlpPfsmFactory.cc

##########
mira_bleu_factory_h= 
mira_bleu_factory_defs= stack_dec/MiraBleuFactory.cc

##########
mira_gtm_factory_h= 
mira_gtm_factory_defs= stack_dec/MiraGtmFactory.cc

##########
mira_wer_factory_h= 
mira_wer_factory_defs= stack_dec/MiraWerFactory.cc

##########
mira_chrf_factory_h= 
mira_chrf_factory_defs= stack_dec/MiraChrFFactory.cc

##########
kb_mira_ll_wu_factory_h= 
kb_mira_ll_wu_factory_defs= stack_dec/KbMiraLlWuFactory.cc

##########
dict_feat__phrscoreinfo_factory_h= 
dict_feat__phrscoreinfo_factory_defs=		\
stack_dec/DictFeatPhrScoreInfoFactory.cc

##########
leveldb_dict_feat__phrscoreinfo_factory_h= 
leveldb_dict_feat__phrscoreinfo_factory_defs=				\
stack_dec/LevelDbDictFeatPhrScoreInfoFactory.cc

##########
translation_metadata__phrscoreinfo_factory_h= 
translation_metadata__phrscoreinfo_factory_defs=	\
stack_dec/TranslationMetadataPhrScoreInfoFactory.cc

##########
json_translation_metadata__phrscoreinfo_factory_h= 
json_translation_metadata__phrscoreinfo_factory_defs=	\
stack_dec/JsonTranslationMetadataPhrScoreInfoFactory.cc

##########
multi_stack_decoder_rec__swli_factory_h= 
multi_stack_decoder_rec__swli_factory_defs=		\
stack_dec/multi_stack_decoder_rec__swli_factory.cc

##########
wg_uncoupled_assisted_trans__swli_factory_h= 
wg_uncoupled_assisted_trans__swli_factory_defs=		\
stack_dec/WgUncoupledAssistedTransSwLiFactory.cc

##########
multi_stack_decoder_rec__pbtm_factory_h= 
multi_stack_decoder_rec__pbtm_factory_defs=		\
stack_dec/multi_stack_decoder_rec__pbtm_factory.cc

##########
wg_uncoupled_assisted_trans__pbtm_factory_h= 
wg_uncoupled_assisted_trans__pbtm_factory_defs=		\
stack_dec/WgUncoupledAssistedTransPbTmFactory.cc

# programs

##########
thot_lm_perp_SOURCES = incr_models/thot_lm_perp.cc
thot_lm_perp_LDFLAGS = libthot.la

##########
thot_ngram_to_leveldb_SOURCES = incr_models/thot_ngram_to_leveldb.cc
thot_ngram_to_leveldb_LDFLAGS = libthot.la

##########
thot_ilm_perp_SOURCES = incr_models/thot_ilm_perp.cc
thot_ilm_perp_LDFLAGS = libthot.la

##########
thot_lm_weight_upd_SOURCES = incr_models/thot_lm_weight_upd.cc
thot_lm_weight_upd_LDFLAGS = libthot.la

##########
thot_calc_swm_lgprob_SOURCES = sw_models/thot_calc_swm_lgprob.cc
thot_calc_swm_lgprob_LDFLAGS = libthot.la

##########
thot_lextable_to_leveldb_SOURCES = sw_models/thot_lextable_to_leveldb.cc
thot_lextable_to_leveldb_LDFLAGS = libthot.la

##########
thot_gen_sw_model_SOURCES = sw_models/thot_gen_sw_model_pars.h	\
sw_models/thot_gen_sw_model.cc
thot_gen_sw_model_LDFLAGS = libthot.la

##########
thot_sort_bin_ilextable_SOURCES = sw_models/thot_sort_bin_ilextable.cc
thot_sort_bin_ilextable_LDFLAGS = libthot.la

##########
thot_sort_bin_ihmmatable_SOURCES = sw_models/thot_sort_bin_ihmmatable.cc
thot_sort_bin_ihmmatable_LDFLAGS = libthot.la

##########
thot_sort_bin_iibm2atable_SOURCES = sw_models/thot_sort_bin_iibm2atable.cc
thot_sort_bin_iibm2atable_LDFLAGS = libthot.la

##########
thot_merge_bin_ilextable_SOURCES = sw_models/thot_merge_bin_ilextable.cc
thot_merge_bin_ilextable_LDFLAGS = libthot.la

##########
thot_merge_bin_ihmmatable_SOURCES = sw_models/thot_merge_bin_ihmmatable.cc
thot_merge_bin_ihmmatable_LDFLAGS = libthot.la

##########
thot_merge_bin_iibm2atable_SOURCES = sw_models/thot_merge_bin_iibm2atable.cc
thot_merge_bin_iibm2atable_LDFLAGS = libthot.la

##########
thot_gen_bin_lex_filter_info_SOURCES = sw_models/thot_gen_bin_lex_filter_info.cc
thot_gen_bin_lex_filter_info_LDFLAGS = libthot.la

##########
thot_filter_bin_ilextable_SOURCES = sw_models/thot_filter_bin_ilextable.cc
thot_filter_bin_ilextable_LDFLAGS = libthot.la

##########
thot_prune_bin_ilextable_SOURCES = sw_models/thot_prune_bin_ilextable.cc
thot_prune_bin_ilextable_LDFLAGS = libthot.la

##########
thot_alig_op_SOURCES = phrase_models/thot_alig_op.cc
thot_alig_op_LDFLAGS = libthot.la

##########
thot_query_pm_SOURCES = phrase_models/thot_query_pm.cc
thot_query_pm_LDFLAGS = libthot.la

##########
thot_gen_phr_model_SOURCES = phrase_models/thot_gen_phr_model.cc
thot_gen_phr_model_LDFLAGS = libthot.la

##########
thot_ttable_to_fbdb_SOURCES = phrase_models/thot_ttable_to_fbdb.cc
thot_ttable_to_fbdb_LDFLAGS = libthot.la

##########
thot_ttable_to_leveldb_SOURCES = phrase_models/thot_ttable_to_leveldb.cc
thot_ttable_to_leveldb_LDFLAGS = libthot.la

##########
thot_wg_proc_SOURCES = error_correction/thot_wg_proc_pars.h	\
error_correction/thot_wg_proc.cc
thot_wg_proc_LDFLAGS = libthot.la

##########
thot_dhs_step_by_step_min_SOURCES =		\
downhill_simplex/thot_dhs_step_by_step_min.cc
thot_dhs_step_by_step_min_LDFLAGS = libthot.la

##########
thot_ms_dec_SOURCES = stack_dec/thot_ms_dec.cc
thot_ms_dec_LDFLAGS = libthot.la

##########
thot_ms_alig_SOURCES = stack_dec/thot_ms_alig.cc
thot_ms_alig_LDFLAGS = libthot.la

##########
thot_li_weight_upd_SOURCES = stack_dec/thot_li_weight_upd.cc
thot_li_weight_upd_LDFLAGS = libthot.la

##########
thot_ll_weight_upd_nblist_SOURCES =		\
stack_dec/thot_ll_weight_upd_nblist.cc
thot_ll_weight_upd_nblist_LDFLAGS = libthot.la

##########
thot_client_SOURCES = stack_dec/thot_client_pars.h	\
stack_dec/thot_client.cc
thot_client_LDFLAGS = libthot.la

##########
thot_server_SOURCES = stack_dec/thot_server_pars.h	\
stack_dec/thot_server.cc
thot_server_LDFLAGS = libthot.la

##########
thot_get_srcsents_from_metadata_SOURCES =	\
stack_dec/thot_get_srcsents_from_metadata.cc
thot_get_srcsents_from_metadata_LDFLAGS = libthot.la

##########
thot_check_constraints_SOURCES =	\
stack_dec/thot_check_constraints.cc
thot_check_constraints_LDFLAGS = libthot.la

##########
thot_scorer_SOURCES = stack_dec/thot_scorer.cc
thot_scorer_LDFLAGS = libthot.la

##########
thot_calc_bleu_SOURCES = stack_dec/thot_calc_bleu.cc
thot_calc_bleu_LDFLAGS = libthot.la

##########
if CODE_TESTING
thot_test_SOURCES = testing/thot_test.cc $(testing_h) $(testing_defs)	\
$(hattrie_pm_testing_h) $(hattrie_pm_testing_defs)                      \
$(leveldb_pm_testing_h) $(leveldb_pm_testing_defs)
thot_test_LDFLAGS = libthot.la
endif

# include headers
includedir= $(prefix)/include/$(PACKAGE)
include_HEADERS = $(common_src_h) $(nlp_common_h) $(incr_models_h)	\
$(kenlm_h) $(sw_models_h) $(phrase_models_h) $(bdb_pm_h)		\
$(hattrie_pm_h) $(leveldb_lm_h) $(leveldb_pm_h) $(smt_preproc_h)	\
$(error_correction_h) $(downhill_simplex_h) $(stack_dec_h)		\
$(leveldb_stack_dec_h) $(picojson_h) $(exper_h) $(exper_defs)		\
$(casmacat_engines_h) $(casmacat_aligner_h) $(casmacat_confid_h)

# libraries

##########
libthot_la_SOURCES= $(thot_sources)
libthot_la_LIBADD= $(KENLM_LDADD) $(LGMP) $(ZLIB) $(LDB_CXX)	\
$(LEVELDB_LIB) $(CPPUNIT) $(LPTHREAD) -ldl

##########
word_penalty_model_factory_la_SOURCES= $(word_penalty_model_factory_h)	\
$(word_penalty_model_factory_defs)
word_penalty_model_factory_la_LIBADD= libthot.la
word_penalty_model_factory_la_LDFLAGS= -module

##########
incr_jel_mer_ngram_lm_factory_la_SOURCES=	\
$(incr_jel_mer_ngram_lm_factory_h)		\
$(incr_jel_mer_ngram_lm_factory_defs)
incr_jel_mer_ngram_lm_factory_la_LIBADD= libthot.la
incr_jel_mer_ngram_lm_factory_la_LDFLAGS= -module

##########
incr_jel_mer_leveldb_ngram_lm_factory_la_SOURCES=	\
$(incr_jel_mer_leveldb_ngram_lm_factory_h)		\
$(incr_jel_mer_leveldb_ngram_lm_factory_defs)
incr_jel_mer_leveldb_ngram_lm_factory_la_LIBADD= libthot.la
incr_jel_mer_leveldb_ngram_lm_factory_la_LDFLAGS= -module

##########
if KENLM_LIB_ENABLED
kenlm_factory_la_SOURCES=				\
$(kenlm_factory_h) $(kenlm_factory_defs)
kenlm_factory_la_LIBADD= libthot.la $(KENLM_LIBADD)
kenlm_factory_la_LDFLAGS= -module
endif

##########
smoothed_incr_ibm2_alig_model_factory_la_SOURCES=	\
$(smoothed_incr_ibm2_alig_model_factory_h)		\
$(smoothed_incr_ibm2_alig_model_factory_defs)
smoothed_incr_ibm2_alig_model_factory_la_LIBADD= libthot.la
smoothed_incr_ibm2_alig_model_factory_la_LDFLAGS= -module

##########
incr_hmm_p0_alig_model_factory_la_SOURCES=	\
$(incr_hmm_p0_alig_model_factory_h)		\
$(incr_hmm_p0_alig_model_factory_defs)
incr_hmm_p0_alig_model_factory_la_LIBADD= libthot.la
incr_hmm_p0_alig_model_factory_la_LDFLAGS= -module

##########
incr_leveldb_hmm_p0_alig_model_factory_la_SOURCES=	\
$(incr_leveldb_hmm_p0_alig_model_factory_h)		\
$(incr_leveldb_hmm_p0_alig_model_factory_defs)
incr_leveldb_hmm_p0_alig_model_factory_la_LIBADD= libthot.la
incr_leveldb_hmm_p0_alig_model_factory_la_LDFLAGS= -module

##########
incr_phrase_model_factory_la_SOURCES=	\
$(incr_phrase_model_factory_h)		\
$(incr_phrase_model_factory_defs)
incr_phrase_model_factory_la_LIBADD= libthot.la
incr_phrase_model_factory_la_LDFLAGS= -module

##########
wba_incr_phrase_model_factory_la_SOURCES=	\
$(wba_incr_phrase_model_factory_h)		\
$(wba_incr_phrase_model_factory_defs)
wba_incr_phrase_model_factory_la_LIBADD= libthot.la
wba_incr_phrase_model_factory_la_LDFLAGS= -module

##########
bdb_phrase_model_factory_la_SOURCES= $(bdb_phrase_model_factory_h)	\
$(bdb_phrase_model_factory_defs)
bdb_phrase_model_factory_la_LIBADD= libthot.la
bdb_phrase_model_factory_la_LDFLAGS= -module

##########
fast_bdb_phrase_model_factory_la_SOURCES=	\
$(fast_bdb_phrase_model_factory_h)		\
$(fast_bdb_phrase_model_factory_defs)
fast_bdb_phrase_model_factory_la_LIBADD= libthot.la
fast_bdb_phrase_model_factory_la_LDFLAGS= -module

##########
leveldb_phrase_model_factory_la_SOURCES=				\
$(leveldb_phrase_model_factory_h) $(leveldb_phrase_model_factory_defs)
leveldb_phrase_model_factory_la_LIBADD= libthot.la
leveldb_phrase_model_factory_la_LDFLAGS= -module

##########
pfsm_ecm_for_wg_factory_la_SOURCES= $(pfsm_ecm_for_wg_factory_h)	\
$(pfsm_ecm_for_wg_factory_defs)
pfsm_ecm_for_wg_factory_la_LIBADD= libthot.la
pfsm_ecm_for_wg_factory_la_LDFLAGS= -module

##########
non_pb_ec_model_for_nb_ucat_factory_la_SOURCES=	\
$(non_pb_ec_model_for_nb_ucat_factory_h)	\
$(non_pb_ec_model_for_nb_ucat_factory_defs)
non_pb_ec_model_for_nb_ucat_factory_la_LIBADD= libthot.la
non_pb_ec_model_for_nb_ucat_factory_la_LDFLAGS= -module

##########
wg_processor_for_anlp__pfsm_factory_la_SOURCES=	\
$(wg_processor_for_anlp__pfsm_factory_h)	\
$(wg_processor_for_anlp__pfsm_factory_defs)
wg_processor_for_anlp__pfsm_factory_la_LIBADD= libthot.la
wg_processor_for_anlp__pfsm_factory_la_LDFLAGS= -module

##########
mira_bleu_factory_la_SOURCES= $(mira_bleu_factory_h)	\
$(mira_bleu_factory_defs)
mira_bleu_factory_la_LIBADD= libthot.la
mira_bleu_factory_la_LDFLAGS= -module

##########
mira_gtm_factory_la_SOURCES= $(mira_gtm_factory_h)  \
$(mira_gtm_factory_defs)
mira_gtm_factory_la_LIBADD= libthot.la
mira_gtm_factory_la_LDFLAGS= -module

##########
mira_wer_factory_la_SOURCES= $(mira_wer_factory_h)  \
$(mira_wer_factory_defs)
mira_wer_factory_la_LIBADD= libthot.la
mira_wer_factory_la_LDFLAGS= -module

##########
mira_chrf_factory_la_SOURCES= $(mira_chrf_factory_h)  \
$(mira_chrf_factory_defs)
mira_chrf_factory_la_LIBADD= libthot.la
mira_chrf_factory_la_LDFLAGS= -module

##########
kb_mira_ll_wu_factory_la_SOURCES= $(kb_mira_ll_wu_factory_h)	\
$(kb_mira_ll_wu_factory_defs)
kb_mira_ll_wu_factory_la_LIBADD= libthot.la
kb_mira_ll_wu_factory_la_LDFLAGS= -module

##########
dict_feat__phrscoreinfo_factory_la_SOURCES=	\
$(dict_feat__phrscoreinfo_factory_h)		\
$(dict_feat__phrscoreinfo_factory_defs)
dict_feat__phrscoreinfo_factory_la_LIBADD= libthot.la
dict_feat__phrscoreinfo_factory_la_LDFLAGS= -module

##########
leveldb_dict_feat__phrscoreinfo_factory_la_SOURCES=	\
$(leveldb_dict_feat__phrscoreinfo_factory_h)		\
$(leveldb_dict_feat__phrscoreinfo_factory_defs)
leveldb_dict_feat__phrscoreinfo_factory_la_LIBADD= libthot.la
leveldb_dict_feat__phrscoreinfo_factory_la_LDFLAGS= -module

##########
translation_metadata__phrscoreinfo_factory_la_SOURCES=	\
$(translation_metadata__phrscoreinfo_factory_h)		\
$(translation_metadata__phrscoreinfo_factory_defs)
translation_metadata__phrscoreinfo_factory_la_LIBADD= libthot.la
translation_metadata__phrscoreinfo_factory_la_LDFLAGS= -module

##########
json_translation_metadata__phrscoreinfo_factory_la_SOURCES=	\
$(json_translation_metadata__phrscoreinfo_factory_h)		\
$(json_translation_metadata__phrscoreinfo_factory_defs)
json_translation_metadata__phrscoreinfo_factory_la_LIBADD= libthot.la
json_translation_metadata__phrscoreinfo_factory_la_LDFLAGS= -module

##########
multi_stack_decoder_rec__swli_factory_la_SOURCES=	\
$(multi_stack_decoder_rec__swli_factory_h)		\
$(multi_stack_decoder_rec__swli_factory_defs)
multi_stack_decoder_rec__swli_factory_la_LIBADD= libthot.la
multi_stack_decoder_rec__swli_factory_la_LDFLAGS= -module

##########
wg_uncoupled_assisted_trans__swli_factory_la_SOURCES=	\
$(wg_uncoupled_assisted_trans__swli_factory_h)		\
$(wg_uncoupled_assisted_trans__swli_factory_defs)
wg_uncoupled_assisted_trans__swli_factory_la_LIBADD= libthot.la
wg_uncoupled_assisted_trans__swli_factory_la_LDFLAGS= -module

##########
multi_stack_decoder_rec__pbtm_factory_la_SOURCES=	\
$(multi_stack_decoder_rec__pbtm_factory_h)		\
$(multi_stack_decoder_rec__pbtm_factory_defs)
multi_stack_decoder_rec__pbtm_factory_la_LIBADD= libthot.la
multi_stack_decoder_rec__pbtm_factory_la_LDFLAGS= -module

##########
wg_uncoupled_assisted_trans__pbtm_factory_la_SOURCES=	\
$(wg_uncoupled_assisted_trans__pbtm_factory_h)		\
$(wg_uncoupled_assisted_trans__pbtm_factory_defs)
wg_uncoupled_assisted_trans__pbtm_factory_la_LIBADD= libthot.la
wg_uncoupled_assisted_trans__pbtm_factory_la_LDFLAGS= -module

##########
libthot_casmacat_la_SOURCES= $(casmacat_engines_h)			\
$(casmacat_engines_defs) $(casmacat_aligner_h) $(casmacat_aligner_defs)	\
$(casmacat_confid_h) $(casmacat_confid_defs)
libthot_casmacat_la_LIBADD= libthot.la
