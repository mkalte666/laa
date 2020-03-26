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

#ifndef laa_plot_h
#define laa_plot_h

#include "shared.h"
#include <functional>

using PlotCallback = std::function<double(size_t index)>;

struct PlotConfig {
    // y axis config
    std::string yLabel = "y";
    double yMin = 0.0;
    double yMax = 1.0;
    double yGridInterval = 0.0;
    double yGridHint = 0.0;
    bool yLogscale = false;

    // x axis config
    std::string xLabel = "y";
    double xMin = 0.0;
    double xMax = 1.0;
    double xGridInterval = 0.0;
    double xGridHint = 0.0;
    bool xLogscale = false;

    ImVec2 size = ImVec2(0.0F, 0.0F);
    ImColor color = 0xFFFFFFFFu;

    std::string label = "Plot";

    size_t count;
};

void BeginPlot(const PlotConfig& config) noexcept;
void Plot(PlotCallback callback, ImColor const* col = nullptr) noexcept;
void EndPlot() noexcept;
#endif //laa_plot_h
