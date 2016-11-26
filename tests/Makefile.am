check_PROGRAMS =

common_cflags = \
	$(AM_CFLAGS) \
	$(GNLP_CFLAGS) \
	-I$(top_builddir)/libgnlp

common_ldadd = \
	$(CONTRIB_LDADD) \
	$(top_builddir)/libgnlp/libgnlp-@API_VERSION@.la

# Client
check_PROGRAMS += test-client
test_client_CPPFLAGS = $(common_cppflags)
test_client_CFLAGS = $(common_cflags)
test_client_LDFLAGS = $(common_libs)
test_client_LDADD = $(common_ldadd)
test_client_SOURCES = test-client.c

# Language support
check_PROGRAMS += test-languages
test_languages_CPPFLAGS = $(common_cppflags)
test_languages_CFLAGS = $(common_cflags)
test_languages_LDFLAGS = $(common_libs)
test_languages_LDADD = $(common_ldadd)
test_languages_SOURCES = test-languages.c

# Speak
check_PROGRAMS += test-speak
test_speak_CPPFLAGS = $(common_cppflags)
test_speak_CFLAGS = $(common_cflags)
test_speak_LDFLAGS = $(common_libs)
test_speak_LDADD = $(common_ldadd)
test_speak_SOURCES = test-speak.c

TESTS = $(check_PROGRAMS)

-include $(top_srcdir)/git.mk
