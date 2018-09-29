chmod +x autogen.sh
./autogen.sh
mkdir -p ${CHRONICLE_TRIPLET}
cd ${CHRONICLE_TRIPLET}
../configure && make