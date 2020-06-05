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

#include "statefilter.h"

#include "../dsp/avg.h"

StateFilterConfig::StateFilterConfig() noexcept
{
    avgMagnitudes.resize(LAA_MAX_FFT_AVG, RealVec());
    for (size_t i = 0; i < LAA_MAX_FFT_AVG; i++) {
        avgMagnitudes[i].resize(LAA_MAX_FFT_LENGTH);
    }
}
void StateFilterConfig::clearAvg() noexcept
{
    for (size_t i = 0; i < LAA_MAX_FFT_AVG; i++) {
        for (size_t j = 0; j < LAA_MAX_FFT_LENGTH; j++) {
            avgMagnitudes[i][j] = 0.0;
        }
    }
}

void StateFilterConfig::makeAvg(RealVec& inOut, size_t fftLen) noexcept
{
    if (avgCount == 0) {
        return;
    }

    if (fftLen != lastFftLen) {
        clearAvg();
        lastFftLen = fftLen;
    }

    for (size_t i = 0; i < fftLen; i++) {
        avgMagnitudes[currAvg][i] = inOut[i];
        inOut[i] = 0.0;
        for (size_t avgI = 0; avgI < avgCount; avgI++) {
            inOut[i] += avgMagnitudes[avgI][i];
        }
        inOut[i] /= static_cast<double>(avgCount);
    }

    ++currAvg;
    if (currAvg >= avgCount) {
        currAvg = 0;
    }
}