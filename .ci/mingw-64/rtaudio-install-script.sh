#!/bin/bash
cd rtaudio
./autogen.sh --no-configure
export CFLAGS=-Wno-unused-but-set-variable # We need this for the RtAudio build
./configure --host=x86_64-w64-mingw32 --disable-shared --prefix=/usr/local #--prefix=/usr/x86_64-w64-mingw32
make
make install