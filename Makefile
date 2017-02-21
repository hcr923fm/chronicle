WINBOOSTLIBDIR="C:\Program Files\boost\boost_1_63_0\x64\lib"
WINBOOSTHEADERDIR="C:\Program Files\boost\boost_1_63_0\x64\include\boost-1_63"
WINRTAUDIOLIBDIR="C:\Users\calmc\Source\Repos\chronicle\lib"

MINGWBOOSTLIBDIR="C:\Program Files\boost\boost_1_63_0\x64_mingw\lib"

MSYS2BOOSTLIBDIR="C:\Program Files\boost\boost_1_63_0\x64_msys2\lib"
MSYS2BOOSTHEADERDIR="C:\Program Files\boost\boost_1_63_0\x64\include\boost-1_63"
MSYS2RTAUDIOLIBDIR="C:\Users\calmc\Source\Repos\rtaudiobuild\msys2\lib"

MSYSWIN2BOOSTLIBDIR="C:/Program Files/boost/boost_1_63_0/x64_msys2/lib"
MSYS2WINBOOSTHEADERDIR="C:/Program Files/boost/boost_1_63_0/x64/include/boost-1_63"
MSYS2WINRTAUDIOLIBDIR="C:/Users/calmc/Source/Repos/rtaudiobuild/msys2/lib"


windows:
	clang chronicle.cpp RtAudio.cpp -D__WINDOWS_DS__ -D__WINDOWS_WASAPI__ -I$(WINBOOSTHEADERDIR) -lole32 -ldsound -luser32 -llibsndfile-1 -Llib -L$(WINBOOSTLIBDIR)

winbuild:
	cl /EHsc chronicle.cpp RtAudio.cpp -D__WINDOWS_DS__ -D__WINDOWS_WASAPI__ /I $(WINBOOSTHEADERDIR) /link /LIBPATH:$(WINBOOSTLIBDIR) /LIBPATH:lib /IMPLIB:

mingw:
	g++ chronicle.cpp RtAudio.cpp -D__WINDOWS_DS__ -D__WINDOWS_WASAPI__ -std=c++11 -I$(WINBOOSTHEADERDIR) -lole32 -ldsound -luser32 -llibsndfile-1 -llibboost_system-mgw53-1_63 -Llib -L$(MINGWBOOSTLIBDIR)

msys2:
	mkdir -p build
	g++ chronicle.cpp RtAudio.cpp -D__WINDOWS_DS__ -D__WINDOWS_WASAPI__ -std=c++11 -I$(MSYS2BOOSTHEADERDIR) -lole32 -ldsound -luser32 -llibsndfile-1 -llibboost_system-mgw63-1_63 -lrtaudio -lwinmm -luuid -lksuser -Llib -L$(MSYS2BOOSTLIBDIR) -L$(MSYS2RTAUDIOLIBDIR) -o build/chronicle.exe
	cp libs/* build/

msys2win:
	g++ chronicle.cpp RtAudio.cpp -D__WINDOWS_DS__ -D__WINDOWS_WASAPI__ -std=c++11 -I$(MSYS2WINBOOSTHEADERDIR) -lole32 -ldsound -luser32 -llibsndfile-1 -llibboost_system-mgw63-1_63 -lrtaudio -lwinmm -luuid -lksuser -Llib -L$(MSYS2WINBOOSTLIBDIR) -L$(MSYS2WINRTAUDIOLIBDIR) -o build\chronicle.exe

linux:
	g++ chronicle.cpp RtAudio.cpp -D__LINUX_ALSA__ -lpthread `pkg-config --libs --cflags sndfile alsa` -std=c++11

clean:
	rm -rf build/
	rm -f *.obj