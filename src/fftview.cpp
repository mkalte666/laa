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


void FftView::update(StateManager& stateManager)
{
    const auto& liveState = stateManager.getLive();
    std::vector<float> fMag;
    fMag.resize(liveState.avgFftInput.size());
    for (size_t i = 0ul; i < fMag.size(); i++) {
        fMag[i] = static_cast<float>(std::abs(liveState.avgFftInput[i]));
    }


    ImGui::Begin("FFT");
    ImGui::End();
}
