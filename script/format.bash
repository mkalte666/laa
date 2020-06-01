#!/bin/env bash
#    format.bash
#
#    Copyright (C) 2019  Malte Kießling
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

if [ "$#" -lt 1 ]; then
  echo "format.sh runs clang-format and cmake-format recursivly on all given source directories"
  echo "you can use the autoformat cmake function to run this automatically on each build"
  echo "usage: format.sh [-dry] <source dir>"
  echo "    dry: only print the files to format, dont actually format them"
  exit 1
fi

targetDir="$1"
dry=0

if [ "$1" = "-dry" ]; then
  echo "Dry run!"
  dry=1
  targetDir="$2"
fi

targetDir=$(realpath "$targetDir")

cd $targetDir || exit 1

echo "running in $targetDir"

cppFiles=""
cmakeFiles=""

# build the list
# subdir is filled with the input at the end of the loop. this is less likely to explode then a for over a find
while ifs= read -r -d '' subdir
do
  # this grabs c++ files and c files
  cppFiles+=$(find "$subdir" -type f \
        -name "*.cpp" \
        -or -name "*.h" \
        -or -name "*.c" \
        -or -name "*.hpp")" "

  # this one grabs cmake files
  cmakeFiles+=$(find "$subdir" -type f \
        -name "CMakeLists.txt" \
        -or -name "*.cmake")" "
done < <(find . -maxdepth 1 \
    -not -name ".*" \
    -and -not -name "*doc*" \
    -and -not -name "*build*" \
    -and -not -name "*3rdparty*" \
    -and -not -name "*thirdparty*" \
    -and -not -name "*windows*" \
    -and -not -name "*fonts*" \
    -type d -print0)              # print \0 after each for the loop, also only directories

# top level cmake file?
if [ -f "$targetDir/CMakeLists.txt" ]; then
    cmakeFiles+=" $targetDir/CMakeLists.txt"
fi

if [ "$dry" -ne 1 ]; then
  echo "cpp..."
  clang-format -style=file -fallback-style=WebKit -i $cppFiles
  echo "cmake..."
  cmake-format -c $targetDir/.cmake-format -i $cmakeFiles
  echo "done!"
  exit 0
else
  echo "===C/CPP FILES==="
  echo "$cppFiles"
  echo "===CMakeFiles===="
  echo "$cmakeFiles"
fi
