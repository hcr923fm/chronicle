AC_INIT([chronicle], [version-0.1.2], [calmcl1@aol.com])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([chronicle.h])
AC_CONFIG_HEADERS([config.h])
AC_LANG(C++)
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_LIBTOOL
BOOST_REQUIRE([1.68.0])
BOOST_FILESYSTEM(mt)
BOOST_SYSTEM

AC_CHECK_HEADERS([iostream, ctime, chrono, thread, iomanip, csignal, cstring])
PKG_CHECK_MODULES(RTAUDIO, rtaudio)
PKG_CHECK_MODULES(SNDFILE, sndfile)


AC_CONFIG_FILES(Makefile)
AC_OUTPUT()