windows:
	CC = clang
	DFLAGS = -D__WINDOWS_DS__ -D__WINDOWS_WASAPI__ -lole32 -ldsound -luser32 -llibsndfile-1 -Llib
	$(CC) chronicle.cpp RtAudio.cpp $(DFLAGS)

linux:
	g++ chronicle.cpp RtAudio.cpp -D__LINUX_ALSA__ -lasound -lpthread -lsndfile -std=c++11

all:
	$(CC) chronicle.cpp RtAudio.cpp $(DFLAGS)