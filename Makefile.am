bin_PROGRAMS = chronicle
chronicle_SOURCES = chronicle.cpp

ACLOCAL_AMFLAGS = -I .

AM_CFLAGS=$(rtaudio_cflags) $(sndfile_cflags) $(BOOST_CPPFLAGS)
AM_LDFLAGS=$(BOOST_FILESYSTEM_LDFLAGS)
LIBS=$(rtaudio_libs) $(sndfile_libs) $(BOOST_FILESYSTEM_LIBS)