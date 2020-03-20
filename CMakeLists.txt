# LAA
#
# Copyright (C) 2020  Malte Kießling
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or any later version.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.

# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# functions stolen from mkalte666/cpputils
include(3rdparty/sdl2wrap/cmake/funcEnableStrictOptions.cmake)
include(3rdparty/sdl2wrap/cmake/funcEnableClangTidy.cmake)

# disable in-source build. found at
# https://stackoverflow.com/questions/1208681/with-cmake-how-would-you-disable-
# in-source-builds
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set(CMAKE_CXX_STANDARD 17)
project(LAA)

add_subdirectory(3rdparty/sdl2wrap)
add_subdirectory(3rdparty/imgui-cmake-blob)
find_package(OpenGL REQUIRED)

add_executable(laatool
        src/shared.h
        src/main.cpp
        src/staticsdl2wrap.cpp
        src/viewmanager.cpp
        src/viewmanager.h
        src/audiohandler.cpp
        src/audiohandler.h
        src/dsp/whitenoisegenerator.cpp
        src/dsp/whitenoisegenerator.h src/dsp/pinknoisegenerator.cpp src/dsp/pinknoisegenerator.h src/dsp/sinegenerator.cpp src/dsp/sinegenerator.h src/dsp/fft.h src/fftview.cpp src/fftview.h src/dsp/hamming.h src/statemanager.cpp src/statemanager.h src/dsp/avg.h src/plot.h src/plot.cpp)

target_link_libraries(laatool PRIVATE imgui gl3w sdl2wrap::sdl2 OpenGL::GL)
enableStrictOptions(laatool)