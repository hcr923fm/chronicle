chmod +x autogen.sh
./autogen.sh
mkdir -p ${CHRONICLE_TRIPLET}
cd ${CHRONICLE_TRIPLET}
../configure --host=x86_64-w64-mingw32 --disable-shared --enable-static --prefix=/usr/local --with-boost=/usr/local/include/boost-1_68 --with-boost-libdir=/usr/local/lib #--prefix=/usr/x86_64-w64-mingw32
LDFLAGS=--trace
CPPFLAGS=-D_ISOC99_SOURCE
make
