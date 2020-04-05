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

#ifndef laa_peak_h
#define laa_peak_h

#include <algorithm>
#include <vector>

template <class T, class Talloc = std::allocator<T>>
size_t findMax(const std::vector<T, Talloc>& in)
{
    size_t max = 0;
    double maxVal = std::numeric_limits<double>::min();
    for (size_t i = 0; i < in.size(); i++) {
        if (maxVal < in[i]) {
            maxVal = in[i];
            max = i;
        }
    }

    return max;
}

template <class T, class Talloc = std::allocator<T>>
size_t findMin(const std::vector<T, Talloc>& in)
{
    size_t min = 0;
    double minVal = std::numeric_limits<double>::max();
    for (size_t i = 0; i < in.size(); i++) {
        if (minVal > in[i]) {
            minVal = in[i];
            min = i;
        }
    }

    return min;
}

#endif //laa_peak_h
