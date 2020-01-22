#!/bin/bash

cmake .
make
mv bin/Silent_Pass bin/Silent_Pass_x64
./clean.sh

cmake -DCMAKE_TOOLCHAIN_FILE=platforms/linux/linux_i686.toolchain.cmake .
make
mv bin/Silent_Pass bin/Silent_Pass_x32
./clean.sh

cmake -DCMAKE_TOOLCHAIN_FILE=platforms/linux/mingw32.toolchain.cmake .
make
./clean.sh
