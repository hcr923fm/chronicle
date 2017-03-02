FILES=chronicle.cpp screen.cpp RtAudio.cpp

MSYS2WIN32LIBS=-lole32 -ldsound -luser32 -lwinmm -luuid -lksuser

MSYS2WINBOOSTLIBS=-lboost_filesystem-mt -lboost_system-mt
MSYS2LINBOOSTLIBS=-lboost_filesystem -lboost_system

MSYS2LIBS=`pkg-config --cflags --libs sndfile`

msys232:
	mkdir -p build/32
	g++ $(FILES) -std=c++11 $(MSYS2WIN32LIBS) $(MSYS2LIBS) $(MSYS2WINBOOSTLIBS) -lncurses -o build/32/chronicle.exe
	cp libs/32/* build/32
	cp README.md build/32
	cp LICENCE build/32
	cp CHANGELOG.md build/32

msys264:
	mkdir -p build/64
	g++ $(FILES) -std=c++11 $(MSYS2WIN32LIBS) $(MSYS2LIBS) $(MSYS2WINBOOSTLIBS) -lncurses -o build/64/chronicle.exe
	cp libs/64/* build/64/
	cp README.md build/64
	cp LICENCE build/64
	cp CHANGELOG.md build/64

linux:
	mkdir -p build/linux
	g++ $(FILES) -std=c++11 -D__LINUX_ALSA__ -lpthread $(MSYS2LIBS) $(MSYS2LINBOOSTLIBS) `pkg-config --cflags --libs ncurses` -lrtaudio -o build/linux/chronicle
	cp README.md build/linux
	cp LICENCE build/linux
	cp CHANGELOG.md build/linux

clean:
	rm -rf build/
	rm -f *.obj