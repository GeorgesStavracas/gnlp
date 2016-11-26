libexec_PROGRAMS = gnlp-daemon

BUILT_SOURCES = \
	gnlp-resources.c \
	gnlp-resources.h

gnlp_daemon_SOURCES = \
	$(BUILT_SOURCES) \
	gnlp-daemon.c \
	gnlp-daemon.h \
	gnlp-listener.c \
	gnlp-listener.h \
	gnlp-log.c \
	gnlp-log.h \
	gnlp-settings.c \
	gnlp-settings.h \
	gnlp-speaker.c \
	gnlp-speaker.h \
	main.c \
	$(NULL)

gnlp_daemon_CFLAGS = \
	$(GNLP_CFLAGS) \
	$(WARN_CFLAGS) \
	-Wno-unused-function \
	-Wno-unused-parameter \
	-I$(top_builddir)/libgnlp \
	$(NULL)

gnlp_daemon_LDADD = \
	$(GNLP_LIBS) \
	$(top_builddir)/libgnlp/libgnlp-@API_VERSION@.la \
	$(NULL)

gnlp_daemon_LDFLAGS = \
	$(WARN_LDFLAGS) \
	-lm \
	$(NULL)


#-------------------------------------------------------

resource_files = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(top_srcdir)/data --sourcedir=$(top_builddir)/data --generate-dependencies $(top_srcdir)/data/gnlp.gresource.xml)
gnlp-resources.c: $(top_srcdir)/data/gnlp.gresource.xml $(resource_files)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(top_srcdir)/data --sourcedir=$(top_builddir)/data --generate-source --c-name gnlp $(top_srcdir)/data/gnlp.gresource.xml
gnlp-resources.h: $(top_srcdir)/data/gnlp.gresource.xml $(resource_files)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(top_srcdir)/data --sourcedir=$(top_builddir)/data --generate-header --c-name gnlp $(top_srcdir)/data/gnlp.gresource.xml

#-------------------------------------------------------

-include $(top_srcdir)/git.mk
