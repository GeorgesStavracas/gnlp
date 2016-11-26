AC_PREREQ([2.69])


dnl ***********************************************************************
dnl Define Versioning Information
dnl ***********************************************************************
m4_define([major_version],[0])
m4_define([minor_version],[1])
m4_define([micro_version],[0])
m4_define([package_version],[major_version.minor_version.micro_version])
m4_define([bug_report_url],[https://bugzilla.gnome.org/enter_bug.cgi?product=gnlp])
m4_define([api_version],[major_version])

AX_IS_RELEASE([micro-version])


dnl ***********************************************************************
dnl Initialize autoconf
dnl ***********************************************************************
AC_INIT([gnlp],[package_version],[bug_report_url])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([NEWS])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_SUBST([ACLOCAL_AMFLAGS], "-I m4")
AC_CANONICAL_HOST


dnl ***********************************************************************
dnl Make version information available to autoconf files
dnl ***********************************************************************
AC_SUBST([MAJOR_VERSION],major_version)
AC_SUBST([MINOR_VERSION],minor_version)
AC_SUBST([MICRO_VERSION],micro_version)
AC_SUBST([API_VERSION],api_version)


dnl ***********************************************************************
dnl Initialize automake
dnl ***********************************************************************
AM_SILENT_RULES([yes])
AM_INIT_AUTOMAKE([1.11 foreign subdir-objects tar-ustar no-dist-gzip dist-xz -Wno-portability])
AM_MAINTAINER_MODE([enable])
AX_GENERATE_CHANGELOG


dnl ***********************************************************************
dnl Add extra debugging with --enable-debug and --enable-compile-warnings
dnl ***********************************************************************
AX_CHECK_ENABLE_DEBUG([no],[]
                      [G_DISABLE_ASSERT G_DISABLE_CHECKS G_DISABLE_CAST_CHECKS])


dnl ***********************************************************************
dnl Internationalization
dnl ***********************************************************************
GETTEXT_PACKAGE=AC_PACKAGE_TARNAME
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [GETTEXT package name])

AM_GNU_GETTEXT_VERSION([0.19.6])
AM_GNU_GETTEXT([external])


dnl ***********************************************************************
dnl Check for required programs
dnl ***********************************************************************
AC_PROG_CC
AC_PROG_INSTALL
AC_PATH_PROG([GLIB_GENMARSHAL],[glib-genmarshal])
AC_PATH_PROG([GLIB_MKENUMS],[glib-mkenums])
AC_PATH_PROG([GLIB_COMPILE_RESOURCES],[glib-compile-resources])
PKG_PROG_PKG_CONFIG([0.22])
GLIB_GSETTINGS
GOBJECT_INTROSPECTION_CHECK([1.42.0])

VAPIGEN_CHECK


AX_COMPILER_FLAGS


dnl ***********************************************************************
dnl Ensure C11 is Supported
dnl ***********************************************************************
AX_CHECK_COMPILE_FLAG([-std=gnu11],
                      [CFLAGS="$CFLAGS -std=gnu11"],
                      [AC_MSG_ERROR([C compiler cannot compile GNU C11 code])])


dnl ***********************************************************************
dnl Check for required packages
dnl ***********************************************************************
GIO_MIN_VERSION=2.42
GLIB_MIN_VERSION=2.42
GTK_MIN_VERSION=3.20
JULIUS_MIN_VERSION=4.4
SENT_MIN_VERSION=4.4
ESPEAK_NG_MIN_VERSION=1.49.0

PKG_CHECK_MODULES(GNLP, [
	gio-2.0 >= GIO_MIN_VERSION
	gio-unix-2.0 >= GIO_MIN_VERSION
	glib-2.0 >= GLIB_MIN_VERSION
	julius-4 >= JULIUS_MIN_VERSION
	sent-4 >= SENT_MIN_VERSION
	espeak-ng >= ESPEAK_NG_MIN_VERSION
	fontconfig
])

PKG_CHECK_MODULES(TOOLS, [
	gtk+-3.0 >= GTK_MIN_VERSION
])


dnl ***********************************************************************
dnl Initialize Libtool
dnl ***********************************************************************
LT_PREREQ([2.2])
LT_INIT


dnl ***********************************************************************
dnl Support for gtk-doc Documentation Engine
dnl ***********************************************************************
GTK_DOC_CHECK([1.14],[--flavour no-tmpl])
AM_CONDITIONAL(ENABLE_GTK_DOC, test "x$enable_gtk_doc" = "xyes")
AC_ARG_ENABLE(doc-cross-references,
              AS_HELP_STRING([--disable-doc-cross-references],
                             [cross reference symbols from other libraries @<:@default=yes@:>@]),
              enable_doc_cross_references=$enableval,
              enable_doc_cross_references=yes)
AM_CONDITIONAL(ENABLE_DOC_CROSS_REFERENCES, test x$enable_doc_cross_references != xno)


dnl ***********************************************************************
dnl Include external code
dnl ***********************************************************************
m4_include([contrib/gnome-desktop/configure.ac])

GNLP_CFLAGS="$GNLP_CFLAGS -I\$(top_builddir)/contrib/gnome-desktop"
AC_SUBST(GNLP_CFLAGS)

GNLP_LDADD="$GNLP_LDADD \$(top_builddir)/contrib/gnome-desktop/libgnome-desktop-private.la"
AC_SUBST(GNLP_LDADD)

GNLP_LIBS="$GNLP_LIBS \$(top_builddir)/contrib/gnome-desktop/libgnome-desktop-private.la"
AC_SUBST(GNLP_LIBS)


dnl ***********************************************************************
dnl Process .in Files
dnl ***********************************************************************
AC_CONFIG_FILES([
	Makefile

	contrib/Makefile

	data/Makefile
	data/languages/Makefile

	daemon/Makefile

	docs/Makefile
	docs/reference/Makefile
	docs/reference/version.xml

	libgnlp/gnlp.pc
	libgnlp/gnlp-version.h
	libgnlp/Makefile

	tests/Makefile

	tools/Makefile
	tools/gnlp-language-tool/Makefile

	po/Makefile.in
],[],
[API_VERSION='$API_VERSION'])
AC_OUTPUT

echo ""
echo " GNOME Natural Language Parser - ${VERSION}"
echo ""
echo " Paths"
echo ""
echo "	Prefix ................................... : ${prefix}"
echo "	Libdir ................................... : ${libdir}"
echo "	Libexecdir ............................... : ${libexecdir}"
echo ""
echo " Options"
echo ""
echo "	Documentation ............................ : ${enable_gtk_doc}"
echo ""
