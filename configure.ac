AC_PREREQ([2.69])


dnl ***********************************************************************
dnl Define Versioning Information
dnl ***********************************************************************
m4_define([major_version],[0])
m4_define([minor_version],[1])
m4_define([micro_version],[0])
m4_define([package_version],[major_version.minor_version.micro_version])
m4_define([bug_report_url],[https://bugzilla.gnome.org/enter_bug.cgi?product=gnlp])
m4_define([api_version],[0])

AX_IS_RELEASE([micro-version])

dnl ***********************************************************************
dnl Initialize autoconf
dnl ***********************************************************************
AC_INIT([gnlp],[package_version],[bug_report_url])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([data/gnlp.pc.in])
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
AX_COMPILER_FLAGS

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
INTROSPECTION_REQUIRED=1.46.0

AC_PROG_CC
AC_PROG_INSTALL
AC_PATH_PROG([GLIB_GENMARSHAL],[glib-genmarshal])
AC_PATH_PROG([GLIB_MKENUMS],[glib-mkenums])
AC_PATH_PROG([GLIB_COMPILE_RESOURCES],[glib-compile-resources])
AC_PATH_PROG([GDBUS_CODEGEN],[gdbus-codegen])
PKG_PROG_PKG_CONFIG([0.22])
GLIB_GSETTINGS
GOBJECT_INTROSPECTION_CHECK([$INTROSPECTION_REQUIRED])
VAPIGEN_CHECK

AC_SUBST(INTROSPECTION_REQUIRED)


dnl ***********************************************************************
dnl Check for required packages
dnl ***********************************************************************
GIO_REQUIRED=2.46.0
GLIB_REQUIRED=2.46.0
PEAS_REQUIRED=1.16.0

PKG_CHECK_MODULES(GNLP, [
	gio-2.0 >= $GIO_REQUIRED
	gio-unix-2.0 >= $GIO_REQUIRED
	gobject-2.0 >= $GLIB_REQUIRED
	gmodule-2.0 >= $GLIB_REQUIRED
	gobject-introspection-1.0 >= $INTROSPECTION_REQUIRED
	libpeas-1.0 >= $PEAS_REQUIRED
])

AC_SUBST(GIO_REQUIRED)
AC_SUBST(GLIB_REQUIRED)
AC_SUBST(PEAS_REQUIRED)


dnl ***********************************************************************
dnl Initialize Libtool
dnl ***********************************************************************
LT_PREREQ([2.2])
LT_INIT


dnl ***********************************************************************
dnl Process .in Files
dnl ***********************************************************************
AC_CONFIG_FILES([
	Makefile

	src/Makefile
	src/gnlp-version.h

	data/Makefile
	data/gnlp-$API_VERSION.pc:data/gnlp.pc.in

	tests/Makefile

	po/Makefile.in
],[],
[API_VERSION='$API_VERSION'])
AC_OUTPUT


echo ""
echo "      GNOME Natural Language Parser - ${VERSION}"
echo "    ------------------------------------------"
echo ""
echo "       Flags:"
echo "              CFlags: ${GNLP_CFLAGS}"
echo ""
echo "       Options"
echo ""
echo "              Prefix ............................... : ${prefix}"
echo "              Libdir ............................... : ${libdir}"
echo ""
