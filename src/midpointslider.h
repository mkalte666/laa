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
float toSlider(const T& min, const T& max, const T& mid, const T& value) noexcept
{
    // value before the midpoint?
    if (value < mid) {
        return 0.5F * static_cast<float>(value) / static_cast<float>(mid - min);
    }

    // value is after the midpoint
    return 0.5F + 0.5F * static_cast<float>(value - mid) / static_cast<float>(max - mid);
}

template <class T>
T fromSlider(const T& min, const T& max, const T& mid, const float& fVal) noexcept
{
    // value before the midpoint
    if (fVal < 0.5F) {
        return static_cast<T>(2.0F * fVal * static_cast<float>(mid - min));
    }

    return mid + static_cast<T>(2.0F * (fVal - 0.5F) * static_cast<float>(max - mid));
}

/**
 * \brief A Slider that has two linear ranges around a central "midpoint"
 * Essentially, there will be two linear ranges, one from min to mid, and one from mid to max
 * Instead of one big linear or log space. This can make sliders more explicit.
 * Example: Linear space for the range 0..20000 looses alot of accuracy in the lower values
 *          Log scale however spends, for some applications, to much time in the lower values
 *          So now you can lineary go from 0 to 1000, and then from 1000 to 20000 in the second half of the slider
 * \tparam T The value type
 * \param label The label
 * \param min minimum value
 * \param max maximum value
 * \param mid the midpoint
 * \param value the value to slode
 */
template <class T, class StrFunc = std::string(T)>
void MidpointSlider(const std::string& label, const T& min, const T& max, const T& mid, T& value, StrFunc strFunc = std::to_string) noexcept
{
    float fVal = toSlider(min, max, mid, value);
    ImGui::SliderFloat(label.c_str(), &fVal, 0.0F, 1.0F, strFunc(value).c_str(), 1.0F);
    value = fromSlider(min, max, mid, fVal);
}

/**
 * \brief Same as MidpointSlider, but for a VSlider instead
 * \tparam T
 * \param label The label
 * \param min minimum value
 * \param max maximum value
 * \param mid the midpoint
 * \param value the value to slode
 * \param size size of the vertical slider. Same as VSliderFloat size
 * \param strFunc functino to convert values to strings for the display
 */
template <class T, class StrFunc = std::string(T)>
void VMidpointSlider(const std::string& label, const T& min, const T& max, const T& mid, T& value, const ImVec2& size, StrFunc strFunc = std::to_string) noexcept
{
    float fVal = toSlider(min, max, mid, value);
    ImGui::VSliderFloat(label.c_str(), size, &fVal, 0.0F, 1.0F, strFunc(value).c_str(), 1.0F);
    value = fromSlider(min, max, mid, fVal);
}

#endif //laa_midpointslider_h
