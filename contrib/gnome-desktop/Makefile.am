noinst_LTLIBRARIES = libgnome-desktop-private.la

libgnome_desktop_private_la_SOURCES = \
	cc-common-language.c \
	cc-common-language.h \
	default-input-sources.h \
	gnome-languages.c \
	gnome-languages.h \
	$(NULL)

libgnome_desktop_private_la_CFLAGS = \
	$(GNLP_CFLAGS) \
	$(WARN_CFLAGS) \
	-DLIBLOCALEDIR=\""$(prefix)/lib/locale"\" \
	-DGNOMELOCALEDIR=\""$(datadir)/locale\"" \
	-DISO_CODES_PREFIX=\""$(ISO_CODES_PREFIX)"\" \
	$(NULL)

-include $(top_builddir)/git.mk
