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

#include "fftview.h"
#include "dsp/hamming.h"

void FftView::update(StateManager& stateManager, std::string idHint)
{
    (void)stateManager;
    ImGui::Begin((idHint + "Mag").c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);

    const auto& liveState = stateManager.getLive();
    auto data = liveState.fftInput;
    toPolar(data);

    auto size = ImGui::GetWindowContentRegionMax();
    PlotConfig plotConfig;
    plotConfig.size = ImVec2(size.x * 0.9F, size.y * 0.8F);
    plotConfig.yAxisConfig.min = 0.00001;
    plotConfig.yAxisConfig.max = 1000.0;
    plotConfig.yAxisConfig.enableLogScale = true;
    plotConfig.yAxisConfig.gridInterval = 1.0;
    plotConfig.yAxisConfig.gridHint = 1.0;

    plotConfig.label = "Mag";
    //plotConfig.yGridInterval = 10;

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

                return data[idx].real();
            });
    }

    for (auto& state : stateManager.getSaved()) {
        if (!state.visible) {
            continue;
        }
        auto savedData = state.fftInput;
        toPolar(savedData);
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
                return savedData[idx].real();
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
