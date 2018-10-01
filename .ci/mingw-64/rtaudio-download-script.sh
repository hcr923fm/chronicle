#!/bin/bash
git clone http://github.com/thestk/rtaudio
cd rtaudio
./autogen.sh --no-configure
export CFLAGS=-Wno-unused-but-set-variable # We need this for the RtAudio build
./configure --host=x86_64-w64-mingw32 --disable-shared --prefix=/usr/x86_64-w64-mingw32