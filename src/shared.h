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

#ifndef laa_shared_h
#define laa_shared_h

#include <cstdio>

#include <SDL.h>

// clang-format off
#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_sdl.h"
#include "misc/cpp/imgui_stdlib.h"
// clang-format on
#include "imguiplot.h"

#include <GL/gl3w.h>

#include <string>
#include <vector>
#include <random>

#include "dsp/fft.h"
#include "version.h"

template <class T>
T& choose(bool arg, T& trueArg, T& falseArg)
{
    if (arg) {
        return trueArg;
    }

    return falseArg;
}

template <class T>
const T& choose(bool arg, const T& trueArg, const T& falseArg)
{
    if (arg) {
        return trueArg;
    }

    return falseArg;
}

#endif //laa_shared_h
