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

void FftView::update(ImVec2 windowSize, StateManager& stateManager)
{
    const auto& liveState = stateManager.getLive();
    auto phaseInput = liveState.avgFftInput;
    toPolar(phaseInput);

    PlotConfig magConfig;
    magConfig.min = 0.0;
    magConfig.max = 10.0;
    magConfig.count = phaseInput.size() / 2;
    magConfig.size = ImVec2(400.0F, 200.0F);
    magConfig.color = liveState.uniqueCol;
    PlotConfig phaseConfig = magConfig;

    (void)windowSize;
    ImGui::Begin("FFT");
    BeginPlot(magConfig);
    if (liveState.visible) {
        Plot([&phaseInput](size_t idx) {
            return phaseInput[idx].real();
        });
    }

    for (auto& state : stateManager.getSaved()) {
        if (!state.visible) {
            continue;
        }
        auto values = state.avgFftInput;
        toPolar(values);
        Plot([&values](size_t idx) {
            return values[idx].real();
        },
            &state.uniqueCol);
    }
    EndPlot();

    BeginPlot(phaseConfig);
    if (liveState.visible) {
        Plot([&phaseInput](size_t idx) {
            return phaseInput[idx].imag();
        });
    }

    for (auto& state : stateManager.getSaved()) {
        if (!state.visible) {
            continue;
        }
        auto values = state.avgFftInput;
        toPolar(values);
        Plot([&values](size_t idx) {
            return values[idx].imag();
        },
            &state.uniqueCol);
    }
    EndPlot();
    ImGui::End();
}
