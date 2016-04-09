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

gnlp_daemon_SOURCES = \
	daemon/gnlp-client-context.c \
	daemon/gnlp-client-context.h \
	daemon/gnlp-context-manager.c \
	daemon/gnlp-context-manager.h \
	daemon/gnlp-daemon.c \
	daemon/gnlp-daemon.h \
	daemon/gnlp-engine.c \
	daemon/gnlp-engine.h \
	daemon/main.c \
	$(NULL)

gnlp_daemon_CFLAGS = \
	$(GNLP_CFLAGS) \
	$(WARN_CFLAGS) \
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

BUILT_SOURCES = \
	$(gnlp_dbus_daemon_sources) \
	$(NULL)

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
