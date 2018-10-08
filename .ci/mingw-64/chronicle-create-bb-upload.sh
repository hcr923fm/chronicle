b2 authorize_application $BACKBLAZE_APP_KEY_ID $BACKBLAZE_APP_KEY
b2 upload_file ${CHRONICLE_TRIPLET}.tar.gz chronicle-releases ${CHRONICLE_VERSION}/${CHRONICLE_TRIPLET}.tar.gz
b2 upload_file ${CHRONICLE_TRIPLET}.zip chronicle-releases ${CHRONICLE_VERSION}/${CHRONICLE_TRIPLET}.zip