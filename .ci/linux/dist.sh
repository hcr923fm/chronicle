cd chronicle-v1.0.0-linux
make mostlyclean
cp ../LICENCE .
cp ../README.md .
cp ../CHANGELOG.md .
cd ..
tar zcf ${CHRONICLE_TRIPLET}.tar.gz ${CHRONICLE_TRIPLET}