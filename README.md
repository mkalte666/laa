# laa
A free audio analyzer. 

Welcome! If you just want to get this running (on windows, anyway), head over to  [Releases](https://github.com/mkalte666/laa/releases) for the current version. 

## WINDOWS Install

Head over to the [Releases](https://github.com/mkalte666/laa/releases) for the current version. 

There should be a zip with all files for windows for the current version there. 
If you want to build for windows, see below. It's not that easy, but quite doable.

## LINUX Install/Build 
![Linux Build](https://github.com/mkalte666/laa/workflows/Linux%20Build/badge.svg?branch=master)

Linux is where laa is written on, so naturally, building and installing should be rather easy. 

### Dependencies 
The only dependencies of laa should be 
 * SDL2, >= 2.0.8
 * fftw3, >= 3.3.8
 * OpenGL libraries 
 * rtaudio, >= 5.0.0
 * python3 (for building only)
 
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

    # If your librtaudio < 5.1.0:
    cmake -D CMAKE_CXX_FLAGS=-DRTAUDIO500=1 ..
    # Else:
    cmake ..

    make -j
    sudo make install 
    
## WINDOWS Build
![Mingw Windows Build](https://github.com/mkalte666/laa/workflows/Mingw%20Windows%20Build/badge.svg?branch=master)

Building for/on windows can be tricky as gcc and friends tend to be a bit outated. 
Make sure gcc is at least 7.3! 8.1+ is strongly reccomended!

Anyway - for the windows release, the scripts under /windows are used. 
The basic workflow (should also work inside cygwin/msys2/...) is 
  * Install mingw-w64 (for `x86_64-w64-mingw32-gcc` and its friends)
  * Install python3 (only needed for building)
  * clone the repository with `--recursive`
  * cd into `windows/`
  * run `winbuild.bash`
  
Note that running `winbuild.bash` for the first time calls `windeps.bash`, wich downloads and builds the dependencies. 
This will take a bit of time.
`winbuild.bash` also creates a release tar and zip with licences, dlls etc. copied over. 
If you have any trouble, or have tested out stuff from within cygwin or msys2 and can report back for that, please open a issue over here. 

## Thats it!
Thanks for being interested in LAA :D
