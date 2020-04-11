# laa
A free audio analyzer. 

The project is still in development, so enjoy with care :) 

Head over to the [Releases](https://github.com/mkalte666/laa/releases) for the current version. 

## Building 
The only dependencies of laa should be 
 * SDL2, >= 2.0.12
 * fftw3, >= 3.3.8
 * OpenGL libraries 
 * rtaudio, >= 5.1.0
 
The rest is in submodules, so make sure to get those!

    git submodule update --init --recursive

Afterwards, it should be the usual cmake workflow

    mkdir build && cd build
    cmake ..
    make -j
    sudo make install 
    
This will most likely not work in Visual Studio. Windows is supported via mingw-w64. 
There are scripts and a cmake toolchain file and scripts under `windows/`. These are used for the zips over on releases. 
