#!/bin/bash
cd ncurses-6.1
./configure --enable-term-driver --enable-sp-funcs --host=x86_64-w64-mingw32 --with-build-cc=gcc --with-pkg-config-libdir=${PKG_CONFIG_PATH} --enable-pc-files --prefix=/usr/x86_64-w64-mingw32
make
make install