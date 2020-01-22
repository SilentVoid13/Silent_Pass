#!/bin/bash

echo -e "\n[*] Starting Normal building...\n"
cmake .
make
mv bin/Silent_Pass bin/Silent_Pass_x64
./clean.sh

echo -e "\n[*] Starting 32 bit building...\n"
cmake -DCMAKE_TOOLCHAIN_FILE=platforms/linux/linux_i686.toolchain.cmake .
make
mv bin/Silent_Pass bin/Silent_Pass_x32
./clean.sh

echo -e "\n[*] Starting MinGW building...\n"
cmake -DCMAKE_TOOLCHAIN_FILE=platforms/linux/mingw32.toolchain.cmake .
make
./clean.sh
