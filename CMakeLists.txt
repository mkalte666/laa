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

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
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
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(LAA)

# for opengl
cmake_policy(SET CMP0072 NEW)

set(imguiIncludeDir
    ${CMAKE_SOURCE_DIR}/3rdparty/imgui-cmake-blob/imgui/
    CACHE PATH "Path to imgui headers")
add_subdirectory(3rdparty/imgui-cmake-blob)

add_subdirectory(3rdparty/imguiplot)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

# i dont really want to painfully build a find file for these two
find_path(fftwInclude "fftw3.h")
find_library(fftwLib "fftw3")
find_path(rtaudioInclude "rtaudio/RtAudio.h")
find_library(rtaudioLib "rtaudio")

# sources
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
    src/state/state.cpp
    src/state/state.h
    src/state/statedata.h
    src/state/statefilter.cpp
    src/state/statefilter.h
    src/state/statemanager.cpp
    src/state/statemanager.h
    src/version.h
    src/viewmanager.cpp
    src/viewmanager.h)

enablestrictoptions(laatool)

# need those cause we dont have a find package here
target_include_directories(laatool SYSTEM PRIVATE ${fftwInclude}
                                                  ${rtaudioInclude} src/)

if(MINGW)
    # fix: sdl2::SDL2main doesnt work for some reason
    find_library(sdl2mainlib "SDL2main")
    add_definitions(-DNOMINMAX)
    target_link_libraries(
        laatool
        PRIVATE mingw32
                imgui
                gl3w
                ${sdl2mainlib}
                SDL2::SDL2
                OpenGL::GL
                imguiplot
                ${fftwLib}
                m
                stdc++
                ${rtaudioLib}
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
                SDL2::SDL2
                SDL2::SDL2main
                OpenGL::GL
                imguiplot
                ${fftwLib}
                m
                stdc++
                pthread
                ${rtaudioLib})
endif()

install(
    TARGETS laatool
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)
