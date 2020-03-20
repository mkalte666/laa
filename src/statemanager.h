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

#ifndef laa_statemanager_h
#define laa_statemanager_h

#include "audiohandler.h"
#include "dsp/avg.h"
#include "dsp/fft.h"
#include "dsp/hamming.h"

struct State {
    RealVec reference = {};
    RealVec input = {};
    ComplexVec fftReference = {};
    ComplexVec fftInput = {};
    ComplexVec avgFftReference = {};
    ComplexVec avgFftInput = {};
    ComplexVec H = {};
    ComplexVec avgH = {};
    ComplexVec h = {};
};

class StateManager {
public:
    void update(AudioHandler& audioHandler);

    const State& getLive() const noexcept;

private:
    size_t lastFrame = 0;
    State liveState = {};
};

#endif //laa_statemanager_h
