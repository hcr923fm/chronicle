MINGWBOOSTLIBDIR="C:\Program Files\boost\boost_1_63_0\x64_mingw\lib"

MSYS2WINBOOSTLIBS=-lboost_filesystem-mt -lboost_system-mt
MSYS2WIN32LIBS=-lole32 -ldsound -luser32 -lwinmm -luuid -lksuser

MSYS2LIBS=`pkg-config --cflags --libs sndfile rtaudio`


windows:
	clang chronicle.cpp RtAudio.cpp -D__WINDOWS_DS__ -D__WINDOWS_WASAPI__ -I$(WINBOOSTHEADERDIR) -lole32 -ldsound -luser32 -llibsndfile-1 -Llib -L$(WINBOOSTLIBDIR)

winbuild:
	cl /EHsc chronicle.cpp RtAudio.cpp -D__WINDOWS_DS__ -D__WINDOWS_WASAPI__ /I $(WINBOOSTHEADERDIR) /link /LIBPATH:$(WINBOOSTLIBDIR) /LIBPATH:lib /IMPLIB:


msys232:
	mkdir -p build/32
	g++ chronicle.cpp RtAudio.cpp -std=c++11 $(MSYS2WIN32LIBS) $(MSYS2LIBS) $(MSYS2WINBOOSTLIBS) -o build/32/chronicle.exe
	cp libs/32/* build/32
	cp README.MD build/32
	cp LICENCE build/32

msys264:
	mkdir -p build/64
	g++ chronicle.cpp RtAudio.cpp -std=c++11 $(MSYS2WIN32LIBS) $(MSYS2LIBS) $(MSYS2WINBOOSTLIBS) -o build/64/chronicle.exe
	cp libs/64/* build/64/
	cp README.MD build/64
	cp LICENCE build/64

linux:
	mkdir -p build/linux
	g++ chronicle.cpp RtAudio.cpp -D__LINUX_ALSA__ -lpthread `pkg-config --libs --cflags sndfile alsa` -std=c++11
	cp README.MD build/linux
	cp LICENCE build/linux

clean:
	rm -rf build/
	rm -f *.obj