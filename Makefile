windows:
	CC = clang
	DFLAGS = -D__WINDOWS_DS__ -D__WINDOWS_WASAPI__ -lole32 -ldsound -luser32 -llibsoundfile-1
	build

linux:
	CC = g++
	DFLAGS = -D__LINUX_ALSA__ -lasound -lpthread -llibsoundfile-1
	build

build:
	$(CC) chronicle.cpp RtAudio.cpp $(DFLAGS)