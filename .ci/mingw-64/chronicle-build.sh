#!/bin/bash
chmod +x autogen.sh
./autogen.sh
./configure --disable-shared --enable-static --host=x86_64-w64-mingw32 --prefix=/usr/local && make