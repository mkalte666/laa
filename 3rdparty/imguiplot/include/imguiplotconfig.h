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

#ifndef imguiplot_imguiplotconfig_h
#define imguiplot_imguiplotconfig_h

#include <imgui.h>
#include <limits>
#include <string>
#include <vector>

/**
 * \brief The Anti aliasing behaviour of a plot
 * If the number of data points is greater than the number of points in a plot,
 * data points are lost in the display.
 * Often enough that is not important.
 * However, if, for example, peaks are important, and those are only present in one data point...
 */
enum class AntiAliasingBehaviour {
    Nearest, ///< No anti aliasing at all.
    Mean, ///< the mean of all points surrounding the plotted index.
    Min, ///< the minimum of the points surroudning the plotted index.
    Max, ///< the maximum of the points surrounding the plotted index.
    AbsMin, ///< the point with the minimum absolute value.
    AbsMax, ///< the point with the maximum absolute value.
};

/**
 * \brief Config for a single axis.
 */
struct AxisConfig {
    /// axis label
    std::string label = "";
    /// axis minimum value
    double min = 0.0;
    /// axis maximum value
    double max = 1.0;
    /// axis grid interval. Set to non-zero value to draw the grid
    double gridInterval = 0.0;
    /// axis grid hint
    double gridHint = 0.0;
    /// set to true to enable logscale on the x axis. min must be non-zero!
    bool enableLogScale = false;
    /// precision of an axis. Affects double->string conversion for tooltip and axis
    long precision = 5;

    /// move a 0..1 range into logscale
    [[nodiscard]] double logConvert(double value) const noexcept;
    /// move a 0..1 back from logscale
    [[nodiscard]] double logConvertBack(double value) const noexcept;
    /// converts a pixel to a value for use in array access etc.
    [[nodiscard]] double pixelToValue(float pixel, float pixelRange) const noexcept;
    /// converts a value to a pixel
    [[nodiscard]] float valueToPixel(double value, float pixelRange) const noexcept;
    /// checks if a value is in axis range
    [[nodiscard]] bool isInAxisRange(double value) const noexcept;
    /// calculate axis
    [[nodiscard]] std::vector<double> calcGridValues() const noexcept;
};

/**
 * \brief Configures a plot and its axis
 */
struct PlotConfig {
    /// Label/Name of the plot. Used as ID
    std::string label = "Plot";
    /// Size of the plot. Set to 0.0F for it to try fitting the window
    ImVec2 size = ImVec2(0.0F, 0.0F);
    /// line thickness
    float lineThickness = 1.0F;
    /// active plot line thickness
    float activeLineThickness = 2.0;
    /// if any y-value jumps are larger than this, the line is not drawn
    double maxLineJumpDistance = std::numeric_limits<double>::max();

    /// X axis config
    AxisConfig xAxisConfig = {};
    /// Y axis config
    AxisConfig yAxisConfig = {};
};

/**
 * \brief Config for a source
 */
struct PlotSourceConfig {
    /// if this plot is active. Only affects if a tooltip is drawn
    bool active = true;
    /// number of elements in source
    size_t count = 0;
    /// on source[0], this is the value on the x axis
    double xMin = 0.0;
    /// on source[count-1], this is the value on the x axis
    double xMax = 0.0;

    /// transform a value to an array index
    [[nodiscard]] size_t valueToArrayIndex(double value) const noexcept;

    /// transform an array index to a value
    [[nodiscard]] double arrayIndexToValue(size_t arrayIndex) const noexcept;

    /// color
    ImColor color = 0xFFFFFFFF;

    /// anti alising behaviour for this source
    AntiAliasingBehaviour antiAliasingBehaviour = AntiAliasingBehaviour::Nearest;
};

struct PlotMarkerConfig {
    /// color
    ImColor color;
    /// if a line on the x axis is drawn
    bool drawXLine = false;
    /// if a line on the y axis is drawn
    bool drawYLine = false;
    /// if the label should have a background
    bool enableLabelBackground = true;
    /// if the label should be custom
    bool enableCustomLabel = false;
    /// the custom tooltip. Shown if enabled
    std::string customLabel = {};
};

#endif // imguiplot_imguiplot_h