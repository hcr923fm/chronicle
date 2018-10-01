#!/bin/bash
cd boost_1_68_0
./b2 --user-config=user-config.jam toolset=gcc target-os=windows release address-model=64 architecture=x86 install