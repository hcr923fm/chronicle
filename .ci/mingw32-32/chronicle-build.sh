# chmod +x autogen.sh
# ./autogen.sh
mkdir -p ${CHRONICLE_TRIPLET}
cd ${CHRONICLE_TRIPLET}
# ../configure --host=i686-w64-mingw32 --with-boost=/usr/i686-w64-mingw32/include/boost-1_68 --with-boost-libdir=/usr/i686-w64-mingw32/lib --prefix=/usr/i686-w64-mingw32
CPPFLAGS="-D_ISOC99_SOURCE -Wl,--trace"
cmake -DCMAKE_TOOLCHAIN_FILE=../.ci/mingw32-32/win32_toolchain.cmake ..
cmake --build .
