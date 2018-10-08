pip install --upgrade b2
b2 authorize_application $BACKBLAZE_APP_KEY_ID $BACKBLAZE_APP_KEY
b2 upload_file ${CHRONICLE_TRIPLET}.tar.gz chronicle-releases ${CHRONICLE_VERSION}/${CHRONICLE_TRIPLET}.tar.gz
cd ${CHRONICLE_TRIPLET}
b2 upload_file ${CHRONICLE_NAME}-${CHRONICLE_VERSION}.tar.gz chronicle-releases ${CHRONICLE_VERSION}/${CHRONICLE_NAME}-${CHRONICLE_VERSION}-src.tar.gz