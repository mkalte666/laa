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

#include "plot.h"

void FftView::update(StateManager& stateManager, std::string idHint)
{
    ImGui::Begin((idHint + "Mag").c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);

    const auto& liveState = stateManager.getLive();
    auto data = liveState.fftInput;
    toPolar(data);

    auto size = ImGui::GetWindowContentRegionMax();
    PlotConfig plotConfig;
    plotConfig.color = liveState.uniqueCol;
    plotConfig.size = ImVec2(size.x*0.9F, size.y*0.9F);
    plotConfig.count = data.size()/2;
    plotConfig.min = 0.00001;
    plotConfig.max = 1000.0;
    plotConfig.yLogscale = true;

    plotConfig.label = "Mag";
    //plotConfig.yGridInterval = 10;

    plotConfig.valueMin = 0.01;
    plotConfig.valueMax = static_cast<double>(liveState.config.sampleRate)/2.0;
    plotConfig.xGridInterval = 0.0;
    plotConfig.xLogscale = true;
    plotConfig.xLogRef = 1.0;

    BeginPlot(plotConfig);
    if (liveState.visible) {
        Plot([&data](size_t idx) {
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
        Plot([&savedData](size_t idx) {
            if (idx >= savedData.size()) {
                return 0.0;
            }
            return savedData[idx].real();
        }, &state.uniqueCol);
    }
    EndPlot();
    ImGui::End();
}
