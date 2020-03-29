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
    ImGui::Begin((idHint + "Signal").c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);

    const auto& liveState = stateManager.getLive();
    const auto& data = liveState.input;
    (void)data;
    auto size = ImGui::GetWindowContentRegionMax();
    PlotConfig plotConfig;
    plotConfig.label = "Signal";
    plotConfig.size = ImVec2(size.x * 0.9F, size.y * 0.9F);
    plotConfig.yAxisConfig.min = -1.05;
    plotConfig.yAxisConfig.max = 1.05;
    plotConfig.yAxisConfig.gridInterval = 0.25;

    plotConfig.xAxisConfig.min = 0.0 - liveState.config.samplesToSeconds(liveState.config.analysisSamples);
    plotConfig.xAxisConfig.max = 0.0;
    plotConfig.xAxisConfig.gridInterval = 0.1;

    BeginPlot(plotConfig);
    if (liveState.visible) {
        PlotSourceConfig sourceConfig;
        sourceConfig.count = data.size();
        sourceConfig.xMin = 0.0 - liveState.config.samplesToSeconds(liveState.config.analysisSamples);
        sourceConfig.xMax = 0.0;
        sourceConfig.color = liveState.uniqueCol;
        Plot(sourceConfig, [&data](size_t idx) {
            return data[idx];
        });
    }

    for (auto& state : stateManager.getSaved()) {
        if (!state.visible) {
            continue;
        }

        PlotSourceConfig sourceConfig;
        sourceConfig.count = state.input.size();
        sourceConfig.xMin = 0.0 - state.config.samplesToSeconds(liveState.config.analysisSamples);
        sourceConfig.xMax = 0.0;
        sourceConfig.color = state.uniqueCol;
        Plot(sourceConfig, [&state](size_t idx) {
            if (idx >= state.input.size()) {
                return 0.0;
            }
            return state.input[idx];
        });
    }

    EndPlot();
    ImGui::End();
}
