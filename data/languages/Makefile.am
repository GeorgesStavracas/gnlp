INSTALL = @INSTALL@
COPY = cp -r

languagesdir = $(datadir)/gnlp-@API_VERSION@/

install:
	${INSTALL} -d ${languagesdir}
	${COPY} $(top_builddir)/data/languages/*/ $(languagesdir);

uninstall-hook:
	rm -r $(languagesdir);

-include $(top_srcdir)/git.mk
