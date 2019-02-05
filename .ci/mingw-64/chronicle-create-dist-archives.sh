cd ${CHRONICLE_TRIPLET}
make mostlyclean
deps=`x86_64-w64-mingw32-objdump -p chronicle.exe | grep dll | awk '{print $3}'`
more_deps='libFLAC++-6.dll libvorbis-0.dll libFLAC-8.dll libogg-0.dll libsndfile-1.dll libspeex-1.dll libvorbis-0.dll libvorbisenc-2.dll libvorbisfile-3.dll'
deps="$deps $more_deps"
dep_paths=`python .ci/mingw-64/list_full_deps.py`
# for dep in ${deps}
# do
#     echo "Looking for ${dep}..."
#     dep_file=`find /usr -name ${dep} | grep -m 1 .dll`
#     if [ -e ${dep_file} ] && [ ! -z ${dep_file} ]
#     then
#         echo "Copying ${dep_file} to $PWD"
#         cp ${dep_file} .
#     else
#         echo "Couldn't find it, moving on"
#     fi
# done
echo "Found the following deps:"
echo for dep_path in $dep_paths
do
    echo ${dep_path}
done

for dep_path in $dep_paths
do
 if [ -e ${dep_path} ] && [ ! -z ${dep_path} ]
    then
        echo "Copying ${dep_path} to $PWD"
        cp ${dep_path} .
    else
        echo "Couldn't find it, moving on"
    fi
done

cp ../LICENCE .
cp ../README.md .
cp ../CHANGELOG.md .
cd ..
tar zcf ${CHRONICLE_TRIPLET}.tar.gz ${CHRONICLE_TRIPLET}
zip -rq9 ${CHRONICLE_TRIPLET}.zip ${CHRONICLE_TRIPLET}