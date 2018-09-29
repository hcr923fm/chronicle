cd ${CHRONICLE_TRIPLET}
make mostlyclean
deps=`ldd chronicle.exe | awk '$0 1~ /mingw/ {print $3;}'`
echo `ldd chronicle.exe`
for dep in ${deps}
do
    echo "Copying ${dep} to $PWD"
    cp ${dep} .
done
cp ../LICENCE .
cp ../README.md .
cp ../CHANGELOG.md .
cd ..
tar zcf ${CHRONICLE_TRIPLET}.tar.gz ${CHRONICLE_TRIPLET}