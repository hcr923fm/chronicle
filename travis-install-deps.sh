#!/bin/sh
set -ex
wget http://www.music.mcgill.ca/~gary/rtaudio/release/rtaudio-5.0.0.tar.gz
tar xvf rtaudio-*
cd rtaudio*
./configure --with-alsa && make && sudo make install
cd ..
wget https://netix.dl.sourceforge.net/project/lame/lame/3.100/lame-3.100.tar.gz
tar xvf lame-*
cd lame-*
./configure --disable-decoder && make && sudo make install