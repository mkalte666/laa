#!/bin/bash

rm -rf ./build
mkdir -p build/
mkdir -p build/package
mkdir -p build/cmake 

# to build dir 
cd build/cmake || exit
cmake ../../../ -DCMAKE_TOOLCHAIN_FILE=../../toolchain.cmake -DCMAKE_BUILD_TYPE=Release .
make -j 4
cd ../../

# copy exe and deps 
cp build/cmake/laatool.exe build/package/ 
./mingw-copy-deps.sh /usr/x86_64-w64-mingw32/ build/package/laatool.exe
cp ../LICENSE build/package/LICENCE.txt 
cp ../3rdparty/thirdPartyNotes.txt build/package/
cp ../3rdparty/imgui-cmake-blob/imgui/LICENSE.txt build/package/LICENSE.MIT.txt
cp ../3rdparty/LICENSE.ZLIB.txt build/package/
cp ../3rdparty/LICENCE.RtAudio.txt build/package

# pack a zip
zip -r build/package.zip build/package/
