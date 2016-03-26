noinst_PROGRAMS =

common_cflags = \
	-I$(top_srcdir) \
	-I$(top_builddir)/src \
	$(AM_CFLAGS) \
	$(GNLP_CFLAGS)

common_ldadd = \
	$(top_builddir)/src/libgnlp-@API_VERSION@.la

# Init
noinst_PROGRAMS += test-context
test_context_CPPFLAGS = $(common_cppflags)
test_context_CFLAGS = $(common_cflags)
test_context_LDFLAGS = $(common_libs)
test_context_LDADD = $(common_ldadd)
test_context_SOURCES = test-context.c
