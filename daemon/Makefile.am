libexec_PROGRAMS = gnlp-daemon

BUILT_SOURCES = \
	gnlp-enum-types.c \
	gnlp-enum-types.h \
	gnlp-resources.c \
	gnlp-resources.h

gnlp_daemon_SOURCES = \
	$(BUILT_SOURCES) \
	gnlp-daemon.c \
	gnlp-daemon.h \
	gnlp-dialog-state.c \
	gnlp-dialog-state.h \
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

enum_types = \
    gnlp-dialog-state.h \
    gnlp-listener.h

gnlp-enum-types.h: gnlp-enum-types.h.template $(enum_types) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template gnlp-enum-types.h.template $(enum_types)) > $@

gnlp-enum-types.c: gnlp-enum-types.c.template $(enum_types) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template gnlp-enum-types.c.template $(enum_types)) > $@

#-------------------------------------------------------

EXTRA_DIST = \
    gnlp-enum-types.h.template \
    gnlp-enum-types.c.template

-include $(top_srcdir)/git.mk
