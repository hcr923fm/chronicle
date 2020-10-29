mkdir -p __build
cd __build
CHRONICLE_FILEEXTS=(tar.gz zip)
CHRONICLE_BUILDS=(linux win32 win64)

CHRONICLE_MAJOR_VERSION=`awk '/MAJOR/ {print $5}' ../version.h | sed 's/[^0-9]//g'`
CHRONICLE_MINOR_VERSION=`awk '/MINOR/ {print $5}' ../version.h | sed 's/[^0-9]//g'`
CHRONICLE_PATCH_VERSION=`awk '/PATCH/ {print $5}' ../version.h | sed 's/[^0-9]//g'`
CHRONICLE_VERSION=${CHRONICLE_MAJOR_VERSION}.${CHRONICLE_MINOR_VERSION}.${CHRONICLE_PATCH_VERSION}

b2 authorize-account $BACKBLAZE_APP_KEY_ID $BACKBLAZE_APP_KEY

for bld in ${CHRONICLE_BUILDS[*]}
do
    for ext in ${CHRONICLE_FILEEXTS[*]}
    do
        if [[ ${bld} == "win"* ]]
        then
            PLATFORM="win"
        else
            PLATFORM="linux"
        fi

        CHRONICLE_FILE_NAME="chronicle-${CHRONICLE_VERSION}-${bld}-${CIRRUS_BUILD_ID}.${ext}"
        CHRONICLE_URL="https://api.cirrus-ci.com/v1/artifact/github/hcr923fm/chronicle/${PLATFORM}_build/package/__build/chronicle-install/${CHRONICLE_FILE_NAME}"
        echo Downloading ${CHRONICLE_URL}

        wget ${CHRONICLE_URL} -O ${CHRONICLE_FILE_NAME}

        b2 upload_file chronicle-releases ${CHRONICLE_FILE_NAME} ${CHRONICLE_FILE_NAME}
    done
done

CHRONICLE_FILE_NAME="chronicle-${CHRONICLE_VERSION}.deb"
CHRONICLE_URL="https://api.cirrus-ci.com/v1/artifact/github/hcr923fm/chronicle/linux_build/package/__build/${CHRONICLE_FILE_NAME}"
echo Downloading ${CHRONICLE_URL}
wget ${CHRONICLE_URL} -O ${CHRONICLE_FILE_NAME}
b2 upload_file chronicle-releases ${CHRONICLE_FILE_NAME} ${CHRONICLE_FILE_NAME}