#!/bin/bash
chmod +x autogen.sh
./autogen.sh
mkdir -p ${CHRONICLE_TRIPLET}
cd ${CHRONICLE_TRIPLET}
../configure --host=x86_64-w64-mingw32 --disable-shared --enable-static
LDFLAGS=--trace
make
