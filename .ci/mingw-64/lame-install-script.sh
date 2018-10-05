#!/bin/bash
cd lame-3.100
./configure --disable-decoder --disable-shared --enable-static --host=x86_64-w64-mingw32 --prefix=/usr/local #--prefix=/usr/x86_64-w64-mingw32
make
make install