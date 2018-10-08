b2 authorize-account $BACKBLAZE_APP_KEY_ID $BACKBLAZE_APP_KEY
b2 upload_file chronicle-releases ${CHRONICLE_TRIPLET}.tar.gz ${CHRONICLE_VERSION}/${CHRONICLE_TRIPLET}.tar.gz
cd ${CHRONICLE_TRIPLET}
b2 upload_file chronicle-releases ${CHRONICLE_NAME}-${CHRONICLE_VERSION}.tar.gz ${CHRONICLE_VERSION}/${CHRONICLE_NAME}-${CHRONICLE_VERSION}-src.tar.gz