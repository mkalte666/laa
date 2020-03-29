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
#include "dsp/windows.h"

#include <list>

struct State {
    AudioConfig config = {};
    ImColor uniqueCol = 0xFFFFFFFF;
    bool visible = true;
    bool active = false;
    std::string name = "Capture";

    // raw values
    RealVec reference = {};
    RealVec input = {};
    // filtered raw values
    RealVec windowedReference = {};
    RealVec windowedInput = {};

    // operations on the direct fft
    ComplexVec fftReference = {};
    ComplexVec fftInput = {};
    ComplexVec polarFftInput = {};
    ComplexVec frequencyResponse = {};
    ComplexVec impulseResponse = {};

    // operations on the averged fft
    size_t avgCount = 4;
    std::vector<ComplexVec> pastFftReference = {};
    std::vector<ComplexVec> pastFftInput = {};
    ComplexVec avgFftReference = {};
    ComplexVec avgFftInput = {};
    ComplexVec avgPolarFftInput = {};
    ComplexVec avgFrequencyResponse = {};
    ComplexVec avgImpluseResponse = {};
};

ImColor randColor();

class StateManager {
public:
    StateManager() noexcept;
    ~StateManager() noexcept = default;
    void update(AudioHandler& audioHandler);

    const State& getLive() const noexcept;

    const std::list<State>& getSaved() const noexcept;

private:
    void deactivateAll();
    void resetAvg();
    size_t lastFrame = 0;
    State liveState = {};

    std::list<State> saved;
};

#endif //laa_statemanager_h
