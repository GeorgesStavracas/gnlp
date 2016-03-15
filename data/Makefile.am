# pkg-config data
# Note that the template file is called gnlp.pc.in, but generates a
# versioned .pc file using some magic in AC_CONFIG_FILES.
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gnlp-$(API_VERSION).pc

DISTCLEANFILES = $(pkgconfig_DATA)
EXTRA_DIST = gnlp.pc.in

-include $(top_srcdir)/git.mk
