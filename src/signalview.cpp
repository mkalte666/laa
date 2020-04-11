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

#include "signalview.h"

void SignalView::update(StateManager& stateManager, std::string idHint) noexcept
{
    ImGui::BeginChild((idHint + "Signal").c_str());

    const auto& liveState = stateManager.getLive();
    const auto& data = liveState.input;
    (void)data;
    auto size = ImGui::GetWindowContentRegionMax();
    PlotConfig plotConfig;
    plotConfig.label = "Signal";
    plotConfig.size = ImVec2(size.x * 0.9F, size.y * 0.8F);
    plotConfig.yAxisConfig.min = -1.05;
    plotConfig.yAxisConfig.max = 1.05;
    plotConfig.yAxisConfig.gridInterval = 0.25;

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
    if (min == 0.0F) {
        min = static_cast<float>(0.0 - liveState.fftDuration);
    }
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

    plotConfig.xAxisConfig.min = static_cast<double>(min);
    plotConfig.xAxisConfig.max = 0.0;
    plotConfig.xAxisConfig.gridInterval = 0.05;

    BeginPlot(plotConfig);
    if (liveState.visible) {
        PlotSourceConfig sourceConfig;
        sourceConfig.count = liveState.fftLen;
        sourceConfig.xMin = 0.0 - liveState.fftDuration;
        sourceConfig.xMax = 0.0;
        sourceConfig.color = liveState.uniqueCol;
        sourceConfig.active = liveState.active;
        Plot(
            sourceConfig, [&data](size_t idx) {
                return data[idx];
            });
    }

    for (auto& state : stateManager.getSaved()) {
        if (!state.visible) {
            continue;
        }

        PlotSourceConfig sourceConfig;
        sourceConfig.count = state.fftLen;
        sourceConfig.xMin = 0.0 - state.fftDuration;
        sourceConfig.xMax = 0.0;
        sourceConfig.color = state.uniqueCol;
        sourceConfig.active = state.active;
        Plot(
            sourceConfig, [&state](size_t idx) {
                if (idx >= state.input.size()) {
                    return 0.0;
                }
                return state.input[idx];
            });
    }

    EndPlot();

    ImGui::SliderFloat("Range", &min, static_cast<float>(0.0 - liveState.fftDuration), 0.0F);

    ImGui::EndChild();
}
