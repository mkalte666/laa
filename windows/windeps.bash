#!/bin/bash

# error reporting during builds
set -e
print_error() {
  echo "Error while building. Check the build.log in the build folder"
}
trap 'print_error $LINENO' ERR


# config

cc_name="$(command -v x86_64-w64-mingw32-gcc)"
cxx_name="$(command -v x86_64-w64-mingw32-g++)"
rc_name="$(command -v x86_64-w64-mingw32-windres)"

sdl_name="SDL2-2.0.12"
fftw_name="fftw-3.3.8"
rtaudio_name="rtaudio-5.1.0"

sdl_tar="$sdl_name.tar.gz"
fftw_tar="$fftw_name.tar.gz"
rtaudio_tar="$rtaudio_name.tar.gz"

# note for users
echo "If this fails:"
echo "Make sure mingw-w64 is installed and has all usual libs (d3d etc) installed"
echo "Also, you need c++17 support! gcc should be >= 8"

# cleanup
echo "Cleanup old dep download and build"
rm -rf ./deps

root_dir="$(realpath ./)"
mkdir -p ./deps/dl || exit 1
dl_dir="$(realpath ./deps/dl)"
mkdir -p ./deps/build || exit 1
build_dir="$(realpath ./deps/build)"
mkdir -p ./deps/install || exit 1
local_root="$(realpath deps/install)"

# make cmake toolchain file (needed later)
echo "Building toolchain file"
roots="/usr/x86_64-w64-mingw32/ $local_root"
{
  echo "set(CMAKE_SYSTEM_NAME Windows)"
  echo "set(CMAKE_C_COMPILER $cc_name)"
  echo "set(CMAKE_CXX_COMPILER $cxx_name)"
  echo "set(CMAKE_RC_COMPILER $rc_name)"
  echo "set(CMAKE_FIND_ROOT_PATH $roots)"
  echo "set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)"
  echo "set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)"
  echo "set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)"
} >> deps/toolchain.cmake || exit 1

# download dependencies
echo "Downloading dependencies"
cd deps/dl || exit 1
wget --quiet "https://www.libsdl.org/release/$sdl_tar" &
wget --quiet "http://www.fftw.org/$fftw_tar" &
wget --quiet "http://www.music.mcgill.ca/~gary/rtaudio/release/$rtaudio_tar" &
wait
echo "Extracting dependencies"
tar xfz "$sdl_tar"
rm -f "$sdl_tar"
tar xfz "$fftw_tar"
rm -f "$fftw_tar"
tar xfz "$rtaudio_tar"
rm -f "$rtaudio_tar"
cd "$root_dir"

# builds come below

# sdl
echo "Building SDL2"
mkdir -p "$build_dir/$sdl_name"
cd "$build_dir/$sdl_name" || exit 1
{
  cmake "$dl_dir/$sdl_name/" \
    -DCMAKE_TOOLCHAIN_FILE="$root_dir/toolchain.cmake" \
    -DCMAKE_BUILD_TYPE=Release\
    -DCMAKE_INSTALL_PREFIX="$local_root"
  make -j"$(nproc)"
  make install
} >> build.log 2>&1 || exit 1

# fftw
echo "Building fftw"
mkdir -p "$build_dir/$fftw_name"
cd "$build_dir/$fftw_name" || exit 1
{
  "$dl_dir/$fftw_name/configure" \
      --with-our-malloc16\
      --with-windows-f77-mangling\
      --enable-shared\
      --disable-static\
      --enable-threads\
      --with-combined-threads\
      --enable-portable-binary\
      --enable-sse2\
      --with-incoming-stack-boundary=2\
      --host x86_64-w64-mingw32\
      --prefix "$local_root"
  make -j"$(nproc)"
  make install
} >> build.log 2>&1 || exit 1

# rtaudio
echo "Building rtaudio"
mkdir -p "$build_dir/$rtaudio_name"
cd "$build_dir/$rtaudio_name" || exit 1
{
  "$dl_dir/$rtaudio_name/configure" \
    --without-ds\
    --without-asio\
    --host x86_64-w64-mingw32\
    --prefix "$local_root"
  make -j"$(nproc)"
  make install
} >> build.log 2>&1 || exit 1

