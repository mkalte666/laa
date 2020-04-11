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
#ifndef laa_midpointslider_h
#define laa_midpointslider_h

#include "shared.h"

template <class T>
float toSlider(const T& min, const T& max, const T& mid, const T& value)
{
    // value before the midpoint?
    if (value < mid) {
        return 0.5F * static_cast<float>(value) / static_cast<float>(mid - min);
    }

    // value is after the midpoint
    return 0.5F + 0.5F * static_cast<float>(value - mid) / static_cast<float>(max - mid);
}

template <class T>
T fromSlider(const T& min, const T& max, const T& mid, const float& fVal)
{
    // value before the midpoint
    if (fVal < 0.5F) {
        return static_cast<T>(2.0F * fVal * static_cast<float>(mid - min));
    }

    return mid + static_cast<T>(2.0F * (fVal - 0.5F) * static_cast<float>(max - mid));
}

template <class T>
void MidpointSlider(const std::string& label, const T& min, const T& max, const T& mid, T& value)
{
    float fVal = toSlider(min, max, mid, value);
    ImGui::SliderFloat(label.c_str(), &fVal, 0.0F, 1.0F, std::to_string(value).c_str(), 1.0F);
    value = fromSlider(min, max, mid, fVal);
}

#endif //laa_midpointslider_h
