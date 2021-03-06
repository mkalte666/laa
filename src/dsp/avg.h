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

#ifndef laa_avg_h
#define laa_avg_h

#include <vector>

template <class T>
inline void mean(std::vector<T>& dst, const std::vector<T>& in)
{
    for (size_t i = 0ull; i < in.size(); i++) {
        dst[i] = (in[i] + dst[i]) / 2.0;
    }
}

template <class T>
inline void weighted(std::vector<T>& dst, const std::vector<T>& in, double weight)
{
    for (size_t i = 0ull; i < in.size(); i++) {
        dst[i] = in[i] * weight + dst[i] * (1.0 - weight);
    }
}

#endif //laa_avg_h
