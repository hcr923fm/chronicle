# chmod +x autogen.sh
# ./autogen.sh
mkdir -p ${CHRONICLE_TRIPLET}
cd ${CHRONICLE_TRIPLET}
# ../configure --host=x86_64-w64-mingw32 --with-boost=/usr/x86_64-w64-mingw32/include/boost-1_68 --with-boost-libdir=/usr/x86_64-w64-mingw32/lib --prefix=/usr/x86_64-w64-mingw32
CPPFLAGS="-D_ISOC99_SOURCE -Wl,--trace"
cmake -DCMAKE_TOOLCHAIN_FILE=../.ci/mingw32-64/win64_toolchain.cmake ..
cmake --build .
