b2 authorize_application $BACKBLAZE_APP_KEY_ID $BACKBLAZE_APP_KEY
b2 upload_file chronicle-releases ${CHRONICLE_TRIPLET}.tar.gz ${CHRONICLE_VERSION}/${CHRONICLE_TRIPLET}.tar.gz
b2 upload_file chronicle-releases ${CHRONICLE_TRIPLET}.zip ${CHRONICLE_VERSION}/${CHRONICLE_TRIPLET}.zip