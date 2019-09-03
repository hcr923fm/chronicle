#!/bin/bash
cd rtaudio
./autogen.sh --no-configure
export CFLAGS=-Wno-unused-but-set-variable # We need this for the RtAudio build
./configure CXX=/usr/bin/x86_64-w64-mingw32-g++-win32 --host=x86_64-w64-mingw32 --disable-shared --prefix=/usr/x86_64-w64-mingw32
make
make install