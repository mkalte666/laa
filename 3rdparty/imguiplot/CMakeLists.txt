# imguiplot
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

# disable in-source build. found at
# https://stackoverflow.com/questions/1208681/with-cmake-how-would-you-disable-
# in-source-builds
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set(CMAKE_CXX_STANDARD 17)
project(LAA)

set(imguiIncludeDir imgui/ CACHE PATH "Path to imgui headers")

add_library(imguiplot STATIC 
        include/imguiplotconfig.h
        include/imguiplot.h
        src/imguiplot.cpp
        src/imguiplotconfig.cpp
        src/antialiasing.cpp)

target_include_directories(imguiplot SYSTEM PRIVATE ${imguiIncludeDir})
target_include_directories(imguiplot PUBLIC include/)

if(WIN32)
    add_definitions(-DNOMINMAX)
else()
    enableStrictOptions(imguiplot)
endif()
