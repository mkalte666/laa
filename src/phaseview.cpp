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

#include "phaseview.h"
void PhaseView::update(StateManager& stateManager, std::string idHint)
{
    ImGui::Begin((idHint + "Phase").c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);

    const auto& liveState = stateManager.getLive();
    auto& data = liveState.avgPolarFftInput;

    auto size = ImGui::GetWindowContentRegionMax();
    PlotConfig plotConfig;
    plotConfig.size = ImVec2(size.x * 0.9F, size.y * 0.8F);
    plotConfig.yAxisConfig.min = -M_PI;
    plotConfig.yAxisConfig.max = M_PI;
    plotConfig.yAxisConfig.enableLogScale = false;
    plotConfig.yAxisConfig.gridInterval = M_PI / 2;

    plotConfig.label = "Mag";

    plotConfig.xAxisConfig.min = static_cast<float>(min);
    plotConfig.xAxisConfig.max = static_cast<float>(max);
    plotConfig.xAxisConfig.enableLogScale = true;
    plotConfig.xAxisConfig.gridInterval = 0.5;
    plotConfig.xAxisConfig.gridHint = 1000.0;

    BeginPlot(plotConfig);

    if (liveState.visible) {
        PlotSourceConfig sourceConfig;
        sourceConfig.count = data.size() / 2;
        sourceConfig.xMin = 0.0;
        sourceConfig.xMax = static_cast<double>(liveState.config.sampleRate / 2);
        sourceConfig.color = liveState.uniqueCol;
        sourceConfig.active = liveState.active;
        Plot(
            sourceConfig,
            [&data](size_t idx) {
                if (idx >= data.size()) {
                    return 0.0;
                }

                return data[idx].imag();
            });
    }

    for (auto& state : stateManager.getSaved()) {
        if (!state.visible) {
            continue;
        }
        auto& savedData = state.polarFftInput;
        PlotSourceConfig sourceConfig;
        sourceConfig.count = savedData.size() / 2;
        sourceConfig.xMin = 0.0;
        sourceConfig.xMax = static_cast<double>(state.config.sampleRate / 2);
        sourceConfig.color = state.uniqueCol;
        sourceConfig.active = state.active;
        Plot(
            sourceConfig, [&savedData](size_t idx) {
                if (idx >= savedData.size()) {
                    return 0.0;
                }
                return savedData[idx].imag();
            });
    }

    EndPlot();

    ImGui::SliderFloat("min", &min, 30.0F, 20000.0F, "%.1f", 4.0F);
    ImGui::SameLine();
    min = std::clamp(min, 30.0F, 20000.0F);
    ImGui::SliderFloat("max", &max, 30.0F, 20000.0F, "%.1f", 4.0F);
    max = std::clamp(max, min, 20000.0F);

    ImGui::End();
}
