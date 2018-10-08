bin_PROGRAMS = chronicle
chronicle_SOURCES = chronicle.h chronicle.cpp screen.h screen.cpp parse_opts.h parse_opts.cpp

ACLOCAL_AMFLAGS = -I build-aux

AM_CXXFLAGS=$(RTAUDIO_CFLAGS) $(SNDFILE_CFLAGS) $(NCURSES_CFLAGS)
AM_CPPFLAGS=$(BOOST_CPPFLAGS)
chronicle_CXXFLAGS=$(AM_CXXFLAGS) -D_WIN32_WINNT=0x600 -g -std=c++11 -march=native -lpthread -pthread -flto -Wl,--no-as-needed
AM_LDFLAGS=$(BOOST_LDFLAGS)
LIBS=$(RTAUDIO_LIBS) $(SNDFILE_LIBS) $(NCURSES_LIBS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) -lmp3lame