thotdocdir = $(datadir)/$(PACKAGE)/doc
thotdoc_DATA= thot_manual.pdf

EXTRA_DIST = doxygen_src.cfg thot_manual.pdf
#################################
#SUFFIXES = .cfg
#.cfg.o:
#	$(DOXYGEN) $<

#noinst_PROGRAMS = doxydoc
#doxydoc_SOURCES = doxygen_src.cfg

#doxydoc: $(doxydoc_SOURCES)
##################################
#BUILT_SOURCES= ./src
#./src: Makefile
#	$(DOXYGEN) doxygen_src.cfg
#CLEANFILES=./src
##################################
doc: 
	if [ -z '$(DOXYGEN)' ]; then echo "Error: doxygen is not installed" >&2 ; else $(DOXYGEN) doxygen_src.cfg; fi

clean_doc:
	rm -rf ./src
