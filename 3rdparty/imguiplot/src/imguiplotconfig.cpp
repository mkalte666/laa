/*
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

#include "imguiplotconfig.h"

#include <cmath>

double AxisConfig::logConvert(double value) const noexcept
{
    if (enableLogScale) {
        value = std::log((min + (max - min) * value) / min) / std::log(max / min);
    }

    return value;
}

double AxisConfig::logConvertBack(double value) const noexcept
{
    if (enableLogScale) {
        value = (std::pow(max / min, value) * min - min) / (max - min);
    }

    return value;
}

double AxisConfig::pixelToValue(float pixel, float pixelRange) const noexcept
{
    double logified = static_cast<double>(pixel) / static_cast<double>(pixelRange);
    double linear = logConvertBack(logified);

    return min + linear * (max - min);
}

float AxisConfig::valueToPixel(double value, float pixelRange) const noexcept
{
    double linear = (value - min) / (max - min);
    double logified = logConvert(linear);

    return static_cast<float>(std::max(0.0, std::min(1.0, logified))) * pixelRange;
}

bool AxisConfig::isInAxisRange(double value) const noexcept
{
    return (value >= min) && (value <= max);
}

std::vector<double> AxisConfig::calcGridValues() const noexcept
{
    std::vector<double> result;

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
    if (gridInterval == 0.0) {
        return result;
    }

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

    if (enableLogScale) {
        auto valUp = gridHint;
        auto valDown = gridHint;
        for (long i = 1; valUp < max || valDown > min; ++i) {
            if (valUp < max && isInAxisRange(valUp)) {
                result.push_back(valUp);
            }

            if (i != 0 && valDown > min && isInAxisRange(valDown)) {
                result.push_back(valDown);
            }
            valUp = gridHint * std::pow(10, static_cast<double>(i) * gridInterval);
            valDown = gridHint / std::pow(10, static_cast<double>(i) * gridInterval);
        }
        return result;
    }

    auto valUp = gridHint;
    auto valDown = gridHint;
    for (long i = 0; valUp < max || valDown > min; ++i) {
        if (valUp < max) {
            result.push_back(valUp);
        }

        if (i != 0 && valDown > min) {
            result.push_back(valDown);
        }
        valUp = gridHint + static_cast<double>(i) * gridInterval;
        valDown = gridHint - static_cast<double>(i) * gridInterval;
    }

    return result;
}

size_t PlotSourceConfig::valueToArrayIndex(double value) const noexcept
{
    if (value > xMax) {
        return count - 1;
    }

    if (value < xMin) {
        return 0;
    }

    auto dCount = static_cast<double>(count);
    auto dIndex = std::round(dCount * (value - xMin) / (xMax - xMin));
    return static_cast<size_t>(dIndex);
}

double PlotSourceConfig::arrayIndexToValue(size_t arrayIndex) const noexcept
{
    auto dCount = static_cast<double>(count);
    auto dIndex = static_cast<double>(arrayIndex);

    return xMin + (xMax - xMin) * (dIndex / dCount);
}
