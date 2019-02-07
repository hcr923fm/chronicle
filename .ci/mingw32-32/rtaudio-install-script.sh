#!/bin/bash
cd rtaudio
./autogen.sh --no-configure
export CFLAGS=-Wno-unused-but-set-variable # We need this for the RtAudio build
./configure --host=i686-w64-mingw32 --disable-shared --prefix=/usr/i686-w64-mingw32
make
make install