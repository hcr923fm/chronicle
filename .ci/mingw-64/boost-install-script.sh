#!/bin/bash
cd boost_1_68_0
./b2 --user-config=user-config.jam --build-type=complete toolset=gcc target-os=windows link=static release address-model=64 architecture=x86 install