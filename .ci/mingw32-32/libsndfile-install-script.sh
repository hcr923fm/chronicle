#!/bin/bash
cd libsndfile-1.0.28
./configure --host=i686-w64-mingw32 --disable-static --prefix=/usr/i686-w64-mingw32
make
make install