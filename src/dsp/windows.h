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

#ifndef laa_hamming_h
#define laa_hamming_h

#include "../shared.h"
#include <cmath>
#include <vector>

template <class T, class Talloc = std::allocator<T>>
inline void hamming(std::vector<T, Talloc>& out, const std::vector<T, Talloc>& in)
{
    auto M = static_cast<double>(in.size() - 1);
    for (size_t i = 0; i < in.size(); i++) {
        auto di = static_cast<double>(i);
        out[i] = in[i] * (0.54 - 0.46 * std::cos(2.0 * LAA_PI * di / M));
    }
}

template <class T, class Talloc = std::allocator<T>>
inline void blackman(std::vector<T, Talloc>& out, const std::vector<T, Talloc>& in)
{
    auto M = static_cast<double>(in.size() - 1);
    for (size_t i = 0; i < in.size(); i++) {
        auto di = static_cast<double>(i);
        out[i] = in[i] * (0.42 - 0.5 * std::cos(2.0 * LAA_PI * di / M) + 0.08 * std::cos(4.0 * LAA_PI * di / M));
    }
}

template <class T, class Talloc = std::allocator<T>>
inline void noWindow(std::vector<T, Talloc>& out, const std::vector<T, Talloc>& in)
{
    for (size_t i = 0; i < in.size(); i++) {
        out[i] = in[i];
    }
}

#endif //laa_hamming_h
