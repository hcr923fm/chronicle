cd __build

CHRONICLE_MAJOR_VERSION=`awk '/MAJOR/ {print $5}' ../version.h | sed 's/[^0-9]//g'`
CHRONICLE_MINOR_VERSION=`awk '/MINOR/ {print $5}' ../version.h | sed 's/[^0-9]//g'`
CHRONICLE_PATCH_VERSION=`awk '/PATCH/ {print $5}' ../version.h | sed 's/[^0-9]//g'`
export CHRONICLE_TRIPLET=chronicle-${CHRONICLE_MAJOR_VERSION}.${CHRONICLE_MINOR_VERSION}.${CHRONICLE_PATCH_VERSION}-linux
mkdir -p ${CHRONICLE_TRIPLET}

dep_paths=`python ../.ci/list_full_deps.py chronicle${EXEC_FILEEXT}`

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

cp chronicle ${CHRONICLE_TRIPLET}
cp ../LICENCE.md ${CHRONICLE_TRIPLET}
cp ../README.md ${CHRONICLE_TRIPLET}
cp ../CHANGELOG.md ${CHRONICLE_TRIPLET}

zip -rq9 ${CHRONICLE_TRIPLET}.zip ${CHRONICLE_TRIPLET}
tar czf ${CHRONICLE_TRIPLET}.tar.gz ${CHRONICLE_TRIPLET}
rm -rf ${CHRONICLE_TRIPLET}