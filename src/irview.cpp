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

#include "irview.h"
void IrView::update(StateManager& stateManager, std::string idHint)
{
    ImGui::Begin((idHint + "Mag").c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);

    const auto& liveState = stateManager.getLive();

    auto size = ImGui::GetWindowContentRegionMax();
    PlotConfig plotConfig;
    plotConfig.label = "IR View";
    plotConfig.size = ImVec2(size.x * 0.9F, size.y * 0.8F);
    plotConfig.yAxisConfig.min = -0.51;
    plotConfig.yAxisConfig.max = 0.51;
    plotConfig.yAxisConfig.gridInterval = 0.1;

    plotConfig.xAxisConfig.min = -0.05F;
    plotConfig.xAxisConfig.max = static_cast<float>(range);
    plotConfig.xAxisConfig.gridInterval = 0.05;
    if (range < 0.1) {
        plotConfig.xAxisConfig.gridInterval = 0.01;
        plotConfig.xAxisConfig.min = -0.01F;
    }
    BeginPlot(plotConfig);
    if (liveState.visible) {
        PlotSourceConfig sourceConfig;
        sourceConfig.xMin = 0.0;
        sourceConfig.xMax = liveState.fftDuration;
        sourceConfig.color = liveState.uniqueCol;
        sourceConfig.active = liveState.active;
        sourceConfig.count = liveState.fftLen;
        sourceConfig.antiAliasingBehaviour = AntiAliasingBehaviour::AbsMax;
        Plot(
            sourceConfig,
            [&liveState](size_t idx) {
                return liveState.impulseResponse[idx];
            });
    }

    for (auto& state : stateManager.getSaved()) {
        if (!state.visible) {
            continue;
        }

        PlotSourceConfig sourceConfig;
        sourceConfig.count = state.fftLen;
        sourceConfig.xMin = 0.0;
        sourceConfig.xMax = state.fftDuration;
        sourceConfig.color = state.uniqueCol;
        sourceConfig.active = state.active;
        sourceConfig.antiAliasingBehaviour = AntiAliasingBehaviour::AbsMax;
        Plot(
            sourceConfig, [&state](size_t idx) {
                return state.impulseResponse[idx];
            });
    }

    EndPlot();

    ImGui::SliderFloat("Range", &range, 0.0F, static_cast<float>(liveState.fftDuration));

    ImGui::End();
}
