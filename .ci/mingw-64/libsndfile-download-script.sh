#!/bin/bash
wget http://www.mega-nerd.com/libsndfile/files/libsndfile-1.0.28.tar.gz
tar xf libsndfile-1.0.28.tar.gz
cd libsndfile-1.0.28
./configure --host=x86_64-w64-mingw32 