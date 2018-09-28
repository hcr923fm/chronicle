#!/bin/bash
wget https://netix.dl.sourceforge.net/project/lame/lame/3.100/lame-3.100.tar.gz
tar xf lame-*
cd lame-3.100
./configure --disable-decoder --disable-shared --enable-static --host=x86_64-w64-mingw32