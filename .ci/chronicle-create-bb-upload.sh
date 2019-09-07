cd __build
b2 authorize-account $BACKBLAZE_APP_KEY_ID $BACKBLAZE_APP_KEY
b2 upload_file chronicle-releases ${CHRONICLE_TRIPLET}.tar.gz ${CHRONICLE_TRIPLET}.tar.gz
b2 upload_file chronicle-releases ${CHRONICLE_TRIPLET}.zip ${CHRONICLE_TRIPLET}.zip