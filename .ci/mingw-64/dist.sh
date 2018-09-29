cd ${CHRONICLE_TRIPLET}
make mostlyclean
deps=`x86_64-w64-mingw32-objdump -p chronicle.exe | grep .dll | awk '{print $3}'`
echo "Deps:"
echo ${deps}
for dep in ${deps}
do
    dep_file=`find /usr name ${dep} | grep .dll -m 1`
    echo "Copying ${dep_file} to $PWD"
    cp ${dep_file} .
done
cp ../LICENCE .
cp ../README.md .
cp ../CHANGELOG.md .
cd ..
tar zcf ${CHRONICLE_TRIPLET}.tar.gz ${CHRONICLE_TRIPLET}