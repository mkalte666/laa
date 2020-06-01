#!/bin/bash

# error reporting during builds
set -e
print_error() {
  echo "Error while building. Check the build.log in the build folder"
}
trap 'print_error' ERR

# check if deps exsist
if [ ! -d "deps" ]; then
  echo "Dependencies not found. running windeps.bash"
  ./windeps.bash
fi

# config
if [ -z "$HOST" ]; then
  HOST="x86_64-w64-mingw32"
fi
root_dir="$(realpath ./)"
src_dir="$root_dir/../"
deps_dir="$root_dir/deps"
local_root="$deps_dir/install"
toolchain="$deps_dir/toolchain.cmake"
build_dir="$root_dir/build"
package_dir="$root_dir/package"
cpdeps="$root_dir/mingw-copy-deps.sh"

# cleanup
echo "Deleting old build"
rm -rf "$build_dir"
mkdir -p "$build_dir"
rm -rf "$package_dir"
mkdir -p "$package_dir"
rm -f ./*.zip
rm -f ./*.tar.gz

# to build dir
echo "Building laa"
cd "$build_dir" || exit 1
{
  cmake "$src_dir"\
    -DCMAKE_TOOLCHAIN_FILE="$toolchain"\
    -DCMAKE_BUILD_TYPE=Release\
    -DCMAKE_INSTALL_PREFIX="$package_dir"
  make -j"$(nproc)"
  make install
} >> build.log 2>&1 || exit 1

# copy and zip
echo "Copy deps and stuff"
cd "$package_dir"
cp "$src_dir/LICENSE" ./LICENCE.txt
cp "$src_dir/3rdparty/thirdPartyNotes.txt" ./
cp "$src_dir/3rdparty/imgui-cmake-blob/imgui/LICENSE.txt" ./LICENSE.MIT.txt
cp "$src_dir/3rdparty/LICENSE.ZLIB.txt" ./
cp "$src_dir/3rdparty/LICENCE.RtAudio.txt" ./
"$cpdeps" "/usr/$HOST/" bin/laatool.exe
"$cpdeps" "/usr/lib/gcc/$HOST/" bin/laatool.exe
"$cpdeps" "$local_root" bin/laatool.exe

echo "Packing..."
packname="build.win64.$(git rev-parse --short HEAD).$(date +%Y.%m.%d.%H.%M.%S)"
zip -r -q "$packname.zip" ./*
mv "$packname.zip" "$root_dir"
tar cfz "$packname.tar.gz" ./*
mv "$packname.tar.gz" "$root_dir"

echo "done"