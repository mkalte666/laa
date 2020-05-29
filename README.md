# laa
A free audio analyzer. 

Welcome! If you just want to get this running (on windows, anyway), head over to  [Releases](https://github.com/mkalte666/laa/releases) for the current version. 

## WINDOWS Install

Head over to the [Releases](https://github.com/mkalte666/laa/releases) for the current version. 

There should be a zip with all files for windows for the current version there. 
If you want to build for windows, see below. It's not that easy, but quite doable.

## LINUX Install/Build 
Linux is where laa is written on, so naturally, building and installing should be rather easy. 

### Dependencies 
The only dependencies of laa should be 
 * SDL2, >= 2.0.8
 * fftw3, >= 3.3.8
 * OpenGL libraries 
 * rtaudio, >= 5.1.0
 
Other versions might work (especially so for sdl2), so rolling release systems (arch, void, ...) should be fine.
 
On debian/ubuntu, you can install this with the usual methods. On debian-based systems, its something like 
    
    sudo apt install libsdl2-dev libfftw3-dev librtaudio-dev 

### Getting the sources
First you need to get the sources. The easiest way is to just clone the repository with git
    
    git clone --recursive https://github.com/mkalte666/laa.git

There are some submodule that we need.
If you downloaded a release tarball, or didn't clone with `--recursive` then you need to run this in the project directory:
 
    git submodule update --init --recursive

Afterwards, it should be the usual cmake workflow

    cd /place/where/you/put/laa
    mkdir build && cd build
    cmake ..
    make -j
    sudo make install 
    
## WINDOWS Build
Building on windows is not recommended. 
For the windows release, the scripts under /windows are used. 
The basic workflow (should also work inside cygwin/msys2/...) is 
  * Install mingw-w64 (for `x86_64-w64-mingw32-gcc` and its friends)
  * Install the dependencies by building them from source
    * note that opengl should just work(tm), cause sdl2 is neat and windows is easy there
  * Check if the toolchain file (cmake/toolchain.cmake) is fine and edit it if your compilers are, for some reason, named differently
  * run winbuild.bash
  
Aside from calling cmake with the toolchain file, the winbuild.bash also copies over licence files, DLLs and creates a release zip.
