#!/bin/bash
chmod +x autogen.sh
./autogen.sh
mkdir -p ${CHRONICLE_TRIPLET}
cd ${CHRONICLE_TRIPLET}
../configure --disable-shared --enable-static --host=x86_64-w64-mingw32 --prefix=/usr/local && make
