cd __build

if [[ $COMPILER_TRIPLET == "x86_64"* ]]; then export ADDRESS_MODEL=64; else export ADDRESS_MODEL=32; fi;
if [[ $COMPILER_TRIPLET == *"mingw32"* ]]; then export XCOMP_PLATOFRM=win${ADDRESS_MODEL}; else export XCOMP_PLATOFRM=linux; fi;

export CHRONICLE_TRIPLET=chronicle-${CHRONICLE_VERSION}-${XCOMP_PLATOFRM}
mkdir -p ${CHRONICLE_TRIPLET}

dep_paths=`python ../.ci/list_full_deps.py chronicle.exe ${COMPILER_TRIPLET}`

for dep_path in $dep_paths
do
 echo "Looking for ${dep_path}"
 if [ -e ${dep_path} ] && [ ! -z ${dep_path} ]
    then
        echo "Copying ${dep_path} to package directory"
        cp ${dep_path} ${CHRONICLE_TRIPLET}
    else
        echo "Couldn't find it, moving on"
    fi
done

cp chronicle.exe ${CHRONICLE_TRIPLET}
cp ../LICENCE.md ${CHRONICLE_TRIPLET}
cp ../README.md ${CHRONICLE_TRIPLET}
cp ../CHANGELOG.md ${CHRONICLE_TRIPLET}

zip -rq9 ${CHRONICLE_TRIPLET}.zip ${CHRONICLE_TRIPLET}
tar czf ${CHRONICLE_TRIPLET}.tar.gz ${CHRONICLE_TRIPLET}
rm -rf ${CHRONICLE_TRIPLET}