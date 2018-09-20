#!/bin/bash
make distclean
./autogen.sh
mkdir chronicle-v1.0.0-x32
mkdir chronicle-v1.0.0-x64
cd chronicle-v1.0.0-x32
../configure --disable-shared --enable-static --host=i686-w64-mingw32 && make
cp ../LICENCE .
cp ../README.md .
cp ../CHANGELOG.md .
cd ../chronicle-v1.0.0-x64
../configure --disable-shared --enable-static --host=x86_64-w64-mingw32 && make
cp ../LICENCE .
cp ../README.md .
cp ../CHANGELOG.md .
cd ..
make dist