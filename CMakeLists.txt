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
include(cmake/funcEnableStrictOptions.cmake)
include(cmake/funcEnableClangTidy.cmake)

# disable in-source build. found at
# https://stackoverflow.com/questions/1208681/with-cmake-how-would-you-disable-
# in-source-builds
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set(CMAKE_CXX_STANDARD 17)
project(LAA)

cmake_policy(SET CMP0072 NEW)

set(imguiIncludeDir
    ${CMAKE_SOURCE_DIR}/3rdparty/imgui-cmake-blob/imgui/
    CACHE PATH "Path to imgui headers")
add_subdirectory(3rdparty/imgui-cmake-blob)

add_subdirectory(3rdparty/imguiplot)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(
    laatool
    src/audio/audioconfig.cpp
    src/audio/audioconfig.h
    src/audio/audiohandler.cpp
    src/audio/audiohandler.h
    src/audio/audiohandler_processing.cpp
    src/audio/audiohandler_ui.cpp
    src/coherenceview.cpp
    src/coherenceview.h
    src/dsp/avg.h
    src/dsp/fft.h
    src/dsp/peak.h
    src/dsp/pinknoisegenerator.cpp
    src/dsp/pinknoisegenerator.h
    src/dsp/sinegenerator.cpp
    src/dsp/sinegenerator.h
    src/dsp/smoothing.h
    src/dsp/sweepgenerator.cpp
    src/dsp/sweepgenerator.h
    src/dsp/whitenoisegenerator.cpp
    src/dsp/whitenoisegenerator.h
    src/dsp/windows.h
    src/freqview.cpp
    src/freqview.h
    src/irview.cpp
    src/irview.h
    src/magview.cpp
    src/magview.h
    src/main.cpp
    src/midpointslider.h
    src/phaseview.cpp
    src/phaseview.h
    src/shared.h
    src/signalview.cpp
    src/signalview.h
    src/state.cpp
    src/state.h
    src/statemanager.cpp
    src/statemanager.h
    src/version.h
    src/viewmanager.cpp
    src/viewmanager.h)

enablestrictoptions(laatool)

if(MINGW)
    add_definitions(-DNOMINMAX)
    target_link_libraries(
        laatool
        PRIVATE mingw32
                imgui
                gl3w
                SDL2main
                SDL2::SDL2
                OpenGL::GL
                imguiplot
                fftw3
                m
                rtaudio
                ole32
                winmm
                ksuser
                mfplat
                mfuuid
                wmcodecdspuuid
                dsound)
    message("Mingw builds are buggy")
else()
    target_link_libraries(
        laatool
        PRIVATE imgui
                gl3w
                SDL2::SDL2main
                SDL2::SDL2
                OpenGL::GL
                imguiplot
                fftw3
                m
                stdc++fs
                stdc++
                pthread
                rtaudio)
endif()

install(
    TARGETS laatool
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)
