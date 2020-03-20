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

void FftView::update(StateManager& stateManager)
{
    PlotConfig config;
    auto avgFftInput = stateManager.getLive().avgFftInput;
    config.size = ImVec2(500.0F, 300.0F);
    config.min = 0.0F;
    config.max = 10.0F;
    config.count = avgFftInput.size() / 2;

    config.callback = [&](size_t idx) {
        return std::abs(avgFftInput[idx]);
    };

    PlotConfig config2 = config;
    config2.min = -10.0;
    config2.max = 10.0;
    config2.callback = [&](size_t idx) {
        return std::arg(avgFftInput[idx]);
    };

    ImGui::Begin("FFT");
    Plot(config);
    Plot(config2);
    ImGui::End();
}
