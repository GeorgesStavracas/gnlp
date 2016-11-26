lib_LTLIBRARIES = libgnlp-@API_VERSION@.la

BUILT_SOURCES =

libgnlp_@API_VERSION@_la_SOURCES = \
	$(BUILT_SOURCES) \
	gnlp-client.c \
	gnlp-client.h \
	gnlp-language.c \
	gnlp-language.h \
	gnlp-utils.c \
	gnlp-utils.h \
	gnlp-version.h \
	gnlp-voice.c \
	gnlp-voice.h \
	gnlp.h \
	$(NULL)

libgnlp_@API_VERSION@_la_CFLAGS = \
	-DGNLP_COMPILATION \
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

dbus_built_sources = \
	gnlp-dbus-generated.c \
	gnlp-dbus-generated.h \
	gnlp-dbus-generates-doc-org.gnome.Gnlp.xml

BUILT_SOURCES += $(dbus_built_sources)

$(dbus_built_sources) : Makefile.am org.gnome.Gnlp.xml
	$(AM_V_GEN) gdbus-codegen \
		--interface-prefix org.gnome.Gnlp. \
		--c-namespace Gnlp \
		--c-generate-object-manager \
		--generate-c-code gnlp-dbus-generated \
		--generate-docbook gnlp-dbus-generated-doc \
		org.gnome.Gnlp.xml \
		$(NULL)


#-------------------------------------------------------

#
# Export the headers to [prefix]/include/gnlp
#
libgnlp_@API_VERSION@_includedir = $(includedir)/gnlp-@API_VERSION@

nobase_libgnlp_@API_VERSION@_include_HEADERS = \
	gnlp.h \
	gnlp-client.h \
	gnlp-language.h \
	gnlp-utils.h \
	gnlp-version.h \
	$(NULL)

#-------------------------------------------------------

-include $(top_srcdir)/git.mk
