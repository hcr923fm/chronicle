WINBOOSTLIBDIR="C:\Program Files\boost\boost_1_63_0\x64\lib"
WINBOOSTHEADERDIR="C:\Program Files\boost\boost_1_63_0\x64\include\boost-1_63"
WINRTAUDIOLIBDIR="C:\Users\calmc\Source\Repos\chronicle\lib"

MINGWBOOSTLIBDIR="C:\Program Files\boost\boost_1_63_0\x64_mingw\lib"

#MSYSWIN2BOOSTLIBDIR="C:/Program Files/boost/boost_1_63_0/msys2_64/lib"
#MSYS2WINBOOSTHEADERDIR="C:/Program Files/boost/boost_1_63_0/msys2_64/include/boost-1_63/boost"
#MSYS2WINRTAUDIOLIBDIR="C:/Users/calmc/Source/Repos/rtaudiobuild/msys2/lib"
MSYS2WINBOOSTLIBDIR="/c/Program Files/boost/boost_1_63_0/msys2_64/lib"
MSYS2WINBOOSTHEADERDIR="/c/Program Files/boost/boost_1_63_0/msys2_64/include/boost-1_63"
MSYS2WINRTAUDIOLIBDIR="/c/Users/calmc/Source/Repos/rtaudiobuild/msys2/lib"
MSYS2WINBOOSTLIBS=-lboost_system-mgw63-mt-1_63 -lboost_filesystem-mgw63-mt-1_63
MSYS2WIN32LIBS=-lole32 -ldsound -luser32 -lwinmm -luuid -lksuser


windows:
	clang chronicle.cpp RtAudio.cpp -D__WINDOWS_DS__ -D__WINDOWS_WASAPI__ -I$(WINBOOSTHEADERDIR) -lole32 -ldsound -luser32 -llibsndfile-1 -Llib -L$(WINBOOSTLIBDIR)

winbuild:
	cl /EHsc chronicle.cpp RtAudio.cpp -D__WINDOWS_DS__ -D__WINDOWS_WASAPI__ /I $(WINBOOSTHEADERDIR) /link /LIBPATH:$(WINBOOSTLIBDIR) /LIBPATH:lib /IMPLIB:


msys2:
	mkdir -p build
	g++ chronicle.cpp RtAudio.cpp -D__WINDOWS_DS__ -D__WINDOWS_WASAPI__ -std=c++11 -I$(MSYS2WINBOOSTHEADERDIR) $(MSYS2WIN32LIBS) -llibsndfile-1 -llibrtaudio $(MSYS2WINBOOSTLIBS) -Llib -L$(MSYS2WINBOOSTLIBDIR) -L$(MSYS2WINRTAUDIOLIBDIR) -o build/chronicle.exe
	cp libs/* build/

linux:
	g++ chronicle.cpp RtAudio.cpp -D__LINUX_ALSA__ -lpthread `pkg-config --libs --cflags sndfile alsa` -std=c++11

clean:
	rm -rf build/
	rm -f *.obj