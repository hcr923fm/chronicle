AC_INIT([chronicle], [1.0.1, [calmcl1@aol.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([chronicle.h])
AC_CONFIG_HEADERS([config.h])
AC_LANG(C++)
AC_PROG_CPP
AC_PROG_CXX
LT_INIT
AX_BOOST_BASE([1.65],,[AC_MSG_ERROR([Chronicle needs Boost - but it was not found in your system])])
AX_BOOST_FILESYSTEM
AX_BOOST_SYSTEM
dnl BOOST_REQUIRE([1.65.0])
dnl BOOST_FILESYSTEM(mt)
dnl BOOST_SYSTEM

AC_CHECK_HEADERS([cstring])
PKG_CHECK_MODULES(RTAUDIO, rtaudio)
PKG_CHECK_MODULES(SNDFILE, sndfile)
PKG_CHECK_MODULES(NCURSES, ncurses)

AC_CONFIG_FILES(Makefile)
AC_OUTPUT()