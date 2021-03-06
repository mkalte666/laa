/*
* This file is part of LAA
* Copyright (c) 2020 Malte Kießling
*
* This program is free software: you can redistribute it and/or modify
    * it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    * General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef laa_version_h
#define laa_version_h

#include <cstdint>
#include <string>

static constexpr int LaaVersionMajor = 0;
static constexpr int LaaVersionMinor = 3;
static constexpr int LaaVersionPatch = 0;

inline std::string getVersionString()
{
    return std::to_string(LaaVersionMajor)
        + "." + std::to_string(LaaVersionMinor)
        + "." + std::to_string(LaaVersionPatch);
}

#endif //laa_version_h
