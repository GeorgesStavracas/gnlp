SUBDIRS = languages

#DBus service
servicedir = $(datadir)/dbus-1/services

service_in_files = org.gnome.Gnlp.service.in
service_DATA = $(service_in_files:.service.in=.service)

org.gnome.Gnlp.service: org.gnome.Gnlp.service.in
	$(AM_V_GEN)	\
		[ -d $(@D) ] || $(mkdir_p) $(@D) ; \
		sed -e "s|\@libexecdir\@|$(libexecdir)|" $< > $@.tmp && mv $@.tmp $@


-include $(top_srcdir)/git.mk
