# pkg-config data
# Note that the template file is called gnlp.pc.in, but generates a
# versioned .pc file using some magic in AC_CONFIG_FILES.
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gnlp-$(API_VERSION).pc

#
# DBus service
#
servicedir = $(datadir)/dbus-1/services
service_in_files = org.gnome.Nlp.service.in

service_DATA = $(service_in_files:.service.in=.service)

%.service: %.service.in Makefile
	@sed -e "s|\@libexecdir\@|$(libexecdir)|" $< > $@

DISTCLEANFILES = \
	$(pkgconfig_DATA) \
	org.gnome.Nlp.service \
	$(NULL)

EXTRA_DIST = \
	gnlp.pc.in \
	org.gnome.Nlp.service.in \
	$(NULL)

-include $(top_srcdir)/git.mk
