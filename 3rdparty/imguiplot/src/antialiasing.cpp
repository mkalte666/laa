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
#include "imguiplot.h"

double aaNearest(const PlotCallback& callback, const PlotSourceConfig& sourceConfig, double xVal)
{
    size_t arrayIndex = sourceConfig.valueToArrayIndex(xVal);
    return callback(arrayIndex);
}

double aaMean(const std::vector<double>& values)
{
    double result = 0.0;
    for (const auto& v : values) {
        result += v;
    }
    result /= static_cast<double>(values.size());
    return result;
}

double aaMin(const std::vector<double>& values)
{
    double result = std::numeric_limits<double>::max();
    for (const auto& v : values) {
        result = std::min(result, v);
    }
    return result;
}

double aaMax(const std::vector<double>& values)
{
    double result = std::numeric_limits<double>::min();
    for (const auto& v : values) {
        result = std::max(result, v);
    }

    return result;
}

double aaAbsMin(const std::vector<double>& values)
{
    double result = std::numeric_limits<double>::max();
    for (const auto& v : values) {
        if (std::abs(v) < std::abs(result)) {
            result = v;
        }
    }

    return result;
}

double aaAbsMax(const std::vector<double>& values)
{
    double result = 0.0;
    for (const auto& v : values) {
        if (std::abs(v) > std::abs(result)) {
            result = v;
        }
    }

    return result;
}

double getAntiAliasingValue(const PlotCallback& callback, const PlotConfig& config, const PlotSourceConfig& sourceConfig, float x, float width)
{
    auto xVal = config.xAxisConfig.pixelToValue(x, width);

    // check the value range
    if (xVal < sourceConfig.xMin || xVal > sourceConfig.xMax) {
        return 0.0;
    }

    // check if we need to do anything at all
    // cause if count is < width, we have enough pixels on the x axis to display everything
    if (sourceConfig.antiAliasingBehaviour == AntiAliasingBehaviour::Nearest
        || static_cast<float>(sourceConfig.count) <= width) {
        return aaNearest(callback, sourceConfig, xVal);
    }

    // calculate the index bounds
    // xMin is one pixel to the left, xMax is one pixel to the right
    // we then get indexMin, indexMax and indexCenter.
    // the min and max then need to be shifted by half their distance to indexCenter
    // this is because we only want to include the region around the current pixel, not the one from the pixels ones around us.
    // so. First: xmin and xmax
    auto xMin = config.xAxisConfig.pixelToValue(x - 1.0F, width);
    auto xMax = config.xAxisConfig.pixelToValue(x + 1.0F, width);
    // the indices
    size_t indexCenter = sourceConfig.valueToArrayIndex(xVal);
    size_t indexMin = sourceConfig.valueToArrayIndex(xMin);
    size_t indexMax = sourceConfig.valueToArrayIndex(xMax);
    // shift the indices
    indexMin = indexCenter - (indexCenter - indexMin) / 2;
    indexMax = indexCenter + (indexMax - indexCenter) / 2;
    // if we end up with the same index, or something is borked, we got nothing to do
    if (indexMin >= indexMax) {
        return callback(indexCenter);
    }

    // now we get the values that are needed for the anti alising operation
    std::vector<double> values;
    values.resize(indexMax - indexMin, 0.0);
    for (size_t i = 0; i < values.size() && i + indexMin < sourceConfig.count; i++) {
        values[i] = callback(i + indexMin);
    }

    // due to the count() rule, values might be empty
    if (values.empty()) {
        return callback(indexCenter);
    }

    // Select the method.
    switch (sourceConfig.antiAliasingBehaviour) {
    case AntiAliasingBehaviour::Nearest:
        return aaNearest(callback, sourceConfig, xVal);
    case AntiAliasingBehaviour::Mean:
        return aaMean(values);
    case AntiAliasingBehaviour::Min:
        return aaMin(values);
    case AntiAliasingBehaviour::Max:
        return aaMax(values);
    case AntiAliasingBehaviour::AbsMin:
        return aaAbsMin(values);
    case AntiAliasingBehaviour::AbsMax:
        return aaAbsMax(values);
    }

    // should never be reached
    return 0.0;
}