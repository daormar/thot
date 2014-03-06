SUBDIRS= nlp_common incr_models sw_models phrase_models smt_preproc	\
error_correction downhill_simplex stack_dec

bin_SCRIPTS = $(nlp_common_bin_scripts) $(incr_models_bin_scripts)	\
$(sw_models_bin_scripts) $(phrase_models_bin_scripts)			\
$(smt_preproc_bin_scripts) $(error_correction_bin_scripts)		\
$(downhill_simplex_bin_scripts) $(stack_dec_bin_scripts)

nlp_common_bin_scripts= nlp_common/thot_shuffle	\
nlp_common/thot_obtain_info_from_verbose_ppl

incr_models_bin_scripts= incr_models/thot_pbs_get_ngram_counts		\
incr_models/thot_merge_ngram_counts incr_models/thot_get_ngram_counts	\
incr_models/thot_get_ngram_counts_mr incr_models/thot_dhs_trgfunc_jmlm	\
incr_models/thot_dhs_trgfunc_interplm

sw_models_bin_scripts= sw_models/thot_prune_text_ilextable		\
sw_models/thot_pbs_gen_best_sw_alig					\
sw_models/thot_pbs_gen_batch_sw_model					\
sw_models/thot_merge_text_ilextable					\
sw_models/thot_merge_text_iibm2atable					\
sw_models/thot_merge_text_ihmmatable sw_models/thot_get_swm_vocab	\
sw_models/thot-gen-wigauss-sent-len-model				\
sw_models/thot_gen_batch_sw_model_mr sw_models/thot_format_corpus_csl	\
sw_models/thot_filter_text_ilextable

phrase_models_bin_scripts= phrase_models/thot_sort_ttable2		\
phrase_models/thot_remove_st_lengths2					\
phrase_models/thot_put_st_lengths2 phrase_models/thot_pbs_gen_phr_model	\
phrase_models/thot_pbs_gen_batch_phr_model				\
phrase_models/thot_pbs_alig_op phrase_models/thot_merge_seglen_counts	\
phrase_models/thot_merge_counts phrase_models/thot_get_nbest_for_trg	\
phrase_models/thot_gen_phr_model_mr phrase_models/thot_flip_phr		\
phrase_models/thot_filter_ttable					\
phrase_models/thot_filter_ttable_given_corpus				\
phrase_models/thot_cut_ttable

smt_preproc_bin_scripts= 

error_correction_bin_scripts= error_correction/thot_rescore_nbest	\
error_correction/thot_obtain_best_trans_from_nbl			\
error_correction/thot_merge_nbest_list					\
error_correction/thot_export_wordgraph

downhill_simplex_bin_scripts= downhill_simplex/thot_dhs_min

stack_dec_bin_scripts= stack_dec/thot_pbs_dec_ms		\
stack_dec/thot_dhs_smt_trgfunc stack_dec/thot_dhs_cat_trgfunc	\
stack_dec/thot_conf_interv_cat stack_dec/thot_cat_using_client	\
stack_dec/thot_calc_wer stack_dec/thot_get_sys_trans

CLEANFILES = $(bin_SCRIPTS)

EXTRA_DIST = 

# Build using suffix rules (portable)
SUFFIXES= .sh .pl

.sh :
	rm -f $@
	echo "#! " $(BASH) > $@
	echo "pkgname=\""$(PACKAGE_NAME)"\"" >> $@
	echo "version=\""$(PACKAGE_VERSION)"\"" >> $@
	echo "bindir="$(prefix)/bin/ >> $@
	echo "AWK="$(AWK) >> $@
	echo "SORT="$(SORT) >> $@
	echo "sortpars=\""$(sortpars)"\"" >> $@
	echo "sortT=\""$(sortT)"\"" >> $@
	echo "sortm=\""$(sortm)"\"" >> $@
	echo "SPLIT="$(SPLIT) >> $@
	echo "SED="$(SED) >> $@
	echo "GREP="$(GREP) >> $@
	echo "SED="$(SED) >> $@
	echo "UNIQ="$(UNIQ) >> $@
	echo "BASENAME="$(BASENAME) >> $@
	echo "SSH="$(SSH) >> $@
	echo "HEAD="$(HEAD) >> $@
	echo "TAIL="$(TAIL) >> $@
	echo "MKTEMP="$(MKTEMP) >> $@
	echo "QSUB="$(QSUB) >> $@
	echo "QSUB_WORKS=\""$(QSUB_WORKS)"\"" >> $@
	echo "QSUB_TERSE_OPT=\""$(QSUB_TERSE_OPT)"\"" >> $@
	echo "QSTAT="$(QSTAT) >> $@
	echo "QRLS="$(QRLS) >> $@
	cat $< >> $@
	chmod ugo+x $@

.pl :
	$(PERL) -c $<
	rm -f $@
	echo "#! " $(PERL)  > $@
	cat $< >> $@
	chmod ugo+x $@
