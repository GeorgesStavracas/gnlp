lib_LTLIBRARIES = libgnlp-@API_VERSION@.la

libgnlp_@API_VERSION@_la_SOURCES = \
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

-include $(top_srcdir)/git.mk
