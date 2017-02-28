inifilesdir = $(datadir)/$(PACKAGE)/ini_files
inifiles_DATA= standard.ini ibm2.ini kenlm.ini interplm.ini

EXTRA_DIST= $(inifiles_DATA)

install-data-hook:
	for file in $(inifiles_DATA); do \
          $(SED) s'|THOT_LIBDIR|${libdir}|' $$file > $(inifilesdir)/$$file; \
	done
	if [ "${ENABLE_IBM2_ALIG}" = "yes" ]; then \
	  cp $(inifilesdir)/ibm2.ini $(inifilesdir)/master.ini; \
	else \
	  cp $(inifilesdir)/standard.ini $(inifilesdir)/master.ini; \
	fi

## if [ ! -f $(inifilesdir)/master.ini ]; then cp $(inifilesdir)/standard.ini $(inifilesdir)/master.ini; fi \
