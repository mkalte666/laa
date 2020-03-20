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

#include "statemanager.h"

void StateManager::update(AudioHandler& audioHandler)
{
    if (audioHandler.getFrameCount() > lastFrame) {
        lastFrame = audioHandler.getFrameCount();
        audioHandler.getFrame(liveState.reference, liveState.input);
        hamming(liveState.reference);
        hamming(liveState.input);

        liveState.fftReference = fftReal(liveState.reference);
        liveState.fftInput = fftReal(liveState.input);

        liveState.H.resize(liveState.reference.size());
        liveState.avgH.resize(liveState.reference.size());
        liveState.avgFftReference.resize(liveState.reference.size());
        liveState.avgFftInput.resize(liveState.reference.size());

        mean(liveState.avgFftReference, liveState.fftReference);
        mean(liveState.avgFftInput, liveState.fftInput);

        for (size_t i = 0; i < liveState.avgFftInput.size(); i++) {
            liveState.H[i] = liveState.fftReference[i] / liveState.fftInput[i];
        }
        mean(liveState.avgH, liveState.H);

        liveState.h = ifft(liveState.H);
    }
}

const State& StateManager::getLive() const noexcept
{
    return liveState;
}
