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
#include "plot.h"

void SignalView::update(StateManager& stateManager, std::string idHint) noexcept
{
    ImGui::Begin((idHint + "Signal").c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);

    const auto& liveState = stateManager.getLive();
    const auto& data = liveState.input;

    auto size = ImGui::GetWindowContentRegionMax();
    PlotConfig plotConfig;
    plotConfig.color = liveState.uniqueCol;
    plotConfig.size = ImVec2(size.x * 0.9F, size.y * 0.9F);
    plotConfig.count = data.size() / 2;
    plotConfig.yMin = -1.05;
    plotConfig.yMax = 1.05;
    plotConfig.label = "Signal";
    plotConfig.yGridInterval = 0.25;

    plotConfig.xMin = 0.0;
    plotConfig.xMax = 0.0 - liveState.config.samplesToSeconds(liveState.config.analysisSamples);
    plotConfig.xGridInterval = 0.1;

    BeginPlot(plotConfig);
    if (liveState.visible) {
        Plot([&data](size_t idx) {
            return data[idx];
        });
    }
    for (auto& state : stateManager.getSaved()) {
        if (!state.visible) {
            continue;
        }
        Plot([&state](size_t idx) {
            if (idx >= state.input.size()) {
                return 0.0;
            }
            return state.input[idx];
        },
            &state.uniqueCol);
    }
    EndPlot();
    ImGui::End();
}
