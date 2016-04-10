#
# Shared flags
#
AM_CPPFLAGS = \
	-DPACKAGE_LIB_DIR=\""$(pkglibdir)"\" \
	-I$(srcdir)/daemon

#
# Libgnlp
#
lib_LTLIBRARIES = libgnlp-@API_VERSION@.la

BUILT_SOURCES =

libgnlp_@API_VERSION@_la_SOURCES = \
	$(BUILT_SOURCES) \
	gnlp-context.c \
	gnlp-context.h \
	gnlp-version.h \
	gnlp.h \
	$(NULL)

libgnlp_@API_VERSION@_la_CFLAGS = \
	$(GNLP_CFLAGS) \
	$(WARN_CFLAGS) \
	$(NULL)
libgnlp_@API_VERSION@_la_LIBADD = \
	$(GNLP_LIBS) \
	$(NULL)
libgnlp_@API_VERSION@_la_LDFLAGS = \
	$(WARN_LDFLAGS) \
	$(NULL)

#-------------------------------------------------------

#
# Gnlp-daemon
#
libexec_PROGRAMS = gnlp-daemon

BUILT_SOURCES += \
    daemon/gnlp-daemon-resources.c \
    daemon/gnlp-daemon-resources.h

gnlp_daemon_SOURCES = \
	plugins/agenda-event-parser/gnlp-agenda-event-parser-plugin.c \
	daemon/gnlp-client-context.c \
	daemon/gnlp-client-context.h \
	daemon/gnlp-context-manager.c \
	daemon/gnlp-context-manager.h \
	daemon/gnlp-daemon.c \
	daemon/gnlp-daemon.h \
	daemon/gnlp-daemon-resources.c \
	daemon/gnlp-daemon-resources.h \
	daemon/gnlp-engine.c \
	daemon/gnlp-engine.h \
	daemon/main.c \
	$(NULL)

gnlp_daemon_CFLAGS = \
	$(GNLP_CFLAGS) \
	$(WARN_CFLAGS) \
	-Wno-unused-function \
	$(NULL)
gnlp_daemon_LDADD = \
	$(GNLP_LIBS) \
	libgnlp-@API_VERSION@.la \
	$(NULL)
gnlp_daemon_LDFLAGS = \
	$(WARN_LDFLAGS) \
	$(NULL)

#-------------------------------------------------------

#
# DBus generated code
#
gnlp_dbus_daemon_sources = \
	daemon/gnlp-dbus-code.c \
	daemon/gnlp-dbus-code.h \
	daemon/gnlp-dbus-code-org.gnome.Nlp.xml \
	$(NULL)

$(gnlp_dbus_daemon_sources) : Makefile.am daemon/org.gnome.Nlp.xml
	$(AM_V_GEN) gdbus-codegen \
	--interface-prefix org.gnome.Nlp. \
	--c-namespace Gnlp \
	--c-generate-object-manager \
	--generate-c-code daemon/gnlp-dbus-code \
	--generate-docbook daemon/gnlp-dbus-code \
	daemon/org.gnome.Nlp.xml \
	$(NULL)

BUILT_SOURCES += \
	$(gnlp_dbus_daemon_sources) \
	$(NULL)

#-------------------------------------------------------

#
# GResources
#
daemon_resources = $(shell glib-compile-resources --sourcedir=$(srcdir)/resources --generate-dependencies $(srcdir)/resources/daemon.gresource.xml)
daemon/gnlp-daemon-resources.c: $(srcdir)/resources/daemon.gresource.xml $(daemon_resources)
	glib-compile-resources --target=$@ --sourcedir=$(srcdir)/resources/ --generate-source --c-name gnlp $(srcdir)/resources/daemon.gresource.xml
daemon/gnlp-daemon-resources.h: $(srcdir)/resources/daemon.gresource.xml $(daemon_resources)
	glib-compile-resources --target=$@ --sourcedir=$(srcdir)/resources/ --generate-header --c-name gnlp $(srcdir)/resources/daemon.gresource.xml

#-------------------------------------------------------

#
# Export the headers to [prefix]/include/gnlp
#
libgnlp_@API_VERSION@_includedir = $(includedir)/gnlp

nobase_libgnlp_@API_VERSION@_include_HEADERS = \
	daemon/gnlp-engine.h \
	gnlp-context.h \
	gnlp-version.h \
	$(NULL)

-include $(top_srcdir)/git.mk

CLEANFILES = \
	$(BUILT_SOURCES) \
	$(NULL)
