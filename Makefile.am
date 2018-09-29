bin_PROGRAMS = chronicle
chronicle_SOURCES = chronicle.h chronicle.cpp screen.h screen.cpp parse_opts.h parse_opts.cpp spdlog/** RtAudio.* lame/*

ACLOCAL_AMFLAGS = -I build-aux

AM_CXXFLAGS=$(RTAUDIO_CFLAGS) $(SNDFILE_CFLAGS) $(NCURSES_CFLAGS) $(BOOST_CPPFLAGS)
chronicle_CXXFLAGS=$(AM_CXXFLAGS) -D_WIN32_WINNT=0x600 -g -std=gnu++0x -march=native -pthread -flto -Wl,--no-as-needed -Ispdlog/include
AM_LDFLAGS=$(BOOST_FILESYSTEM_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS)
LIBS=$(RTAUDIO_LIBS) $(SNDFILE_LIBS) $(NCURSES_LIBS) $(BOOST_FILESYSTEM_LIBS) $(BOOST_SYSTEM_LIBS) -lmp3lame