#!/bin/bash
cd boost_1_68_0
./bootstrap.sh --with-libraries=system,filesystem --prefix=/usr/i686-w64-mingw32
./b2 --user-config=user-config.jam --build-type=complete --layout=versioned toolset=gcc target-os=windows link=static release address-model=32 architecture=x86 install