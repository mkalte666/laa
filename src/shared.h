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

#define SDL2WRAP_STATIC
#include <sdl2wrap/sdl2stl.h>

// clang-format off
#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_sdl.h"
#include "misc/cpp/imgui_stdlib.h"
// clang-format on

#include <GL/gl3w.h>

#include <string>
#include <complex>
#include <valarray>
#include <random>

namespace s2 = sdl2wrap;

#endif //laa_shared_h
