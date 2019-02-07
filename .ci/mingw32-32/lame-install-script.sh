#!/bin/bash
cd lame-3.100
./configure --disable-decoder --disable-shared --enable-static --host=i686-w64-mingw32 --prefix=/usr/i686-w64-mingw32
make
make install