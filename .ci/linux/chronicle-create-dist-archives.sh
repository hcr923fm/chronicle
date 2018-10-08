cd ${CHRONICLE_TRIPLET}
ldd ./chronicle # Curious to see what the linux version links against
make mostlyclean
cp ../LICENCE .
cp ../README.md .
cp ../CHANGELOG.md .
cd ..
tar zcf ${CHRONICLE_TRIPLET}.tar.gz ${CHRONICLE_TRIPLET}
cd ${CHRONICLE_TRIPLET}
make dist