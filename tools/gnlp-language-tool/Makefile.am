bin_PROGRAMS = gnlp-language-tool

BUILT_SOURCES = \
	glt-resources.c \
	glt-resources.h

gnlp_language_tool_SOURCES = \
	$(BUILT_SOURCES) \
	views/glt-language-view.c \
	views/glt-language-view.h \
	views/glt-selector-row.c \
	views/glt-selector-row.h \
	views/glt-selector-view.c \
	views/glt-selector-view.h \
	glt-application.c \
	glt-application.h \
	glt-view.c \
	glt-view.h \
	glt-view-row.c \
	glt-view-row.h \
	glt-view-selector-popover.c \
	glt-view-selector-popover.h \
	glt-workbench.c \
	glt-workbench.h \
	main.c

gnlp_language_tool_CFLAGS = \
	$(GNLP_CFLAGS) \
	$(TOOLS_CFLAGS) \
	-I$(top_builddir)/libgnlp \
	-Iviews

gnlp_language_tool_LDADD = \
	$(GNLP_LIBS) \
	$(TOOLS_LIBS) \
	$(top_builddir)/libgnlp/libgnlp-@API_VERSION@.la

#-------------------------------------------------------

resource_prefix = $(top_builddir)/tools/gnlp-language-tool

resource_files = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(resource_prefix) --sourcedir=$(resource_prefix) --generate-dependencies $(resource_prefix)/glt.gresource.xml)

glt-resources.c: $(resource_prefix)/glt.gresource.xml $(resource_files)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(resource_prefix) --sourcedir=$(resource_prefix) --generate-source --c-name glt $(resource_prefix)/glt.gresource.xml
glt-resources.h: $(resource_prefix)/glt.gresource.xml $(resource_files)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(resource_prefix) --sourcedir=$(resource_prefix) --generate-header --c-name glt $(resource_prefix)/glt.gresource.xml

#-------------------------------------------------------

-include $(top_srcdir)/git.mk
