inifilesdir = $(datadir)/$(PACKAGE)/ini_files
inifiles_DATA= standard.ini

EXTRA_DIST= $(inifiles_DATA)

install-data-hook:
	for file in $(inifiles_DATA); do \
            $(SED) s'|THOT_HOME|${prefix}|' $$file > $(inifilesdir)/$$file; \
	done
	cp $(inifilesdir)/standard.ini $(inifilesdir)/master.ini
#	if [ ! -f $(inifilesdir)/master.ini ]; then cp $(inifilesdir)/standard.ini $(inifilesdir)/master.ini; fi
