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

#ifndef laa_state_h
#define laa_state_h

#include "dsp/avg.h"
#include "dsp/windows.h"
#include "shared.h"

static constexpr size_t LAA_MAX_FFT_LENGTH = 65536;
static constexpr size_t LAA_MIN_FFT_LENGTH = 512;

struct StateData {
    size_t fftLen = 0;
    // raw input
    RealVec input = {};
    RealVec reference = {};
    // windowed input
    RealVec windowedInput = {};
    RealVec windowedReference = {};
    // unfiltered fft
    ComplexVec fftInput = {};
    ComplexVec fftReference = {};
    // here be things to be used by widgets
    ComplexVec filteredFftInput = {};
    ComplexVec filteredFftReference = {};
    RealVec filteredMagFftInput = {};
    RealVec phaseDelta = {};
    ComplexVec frequencyResponse = {};
    RealVec impulseResponse = {};

    // this is here for convenience, to be filled in in various places
    ImColor uniqueCol = 0xFFFFFFFF;
    std::string name = "";
    bool active = true;
    bool visible = true;
    double fftDuration = 0.0;
    double sampleRate = 0.0;
};

class State {
public:
    State() = delete;
    explicit State(size_t fftLen) noexcept;
    ~State() noexcept;

    State(const State&) noexcept = delete;
    State(State&&) noexcept = delete;
    State& operator=(const State&) noexcept = delete;
    State& operator=(State&&) noexcept = delete;

    void calc() noexcept;

    const StateData& getData() noexcept;
    StateData& accessData() noexcept;

private:
    StateData data = {};
    fftw_plan fftInputPlan = {};
    fftw_plan fftReferencePlan = {};
    fftw_plan impulseResponsePlan = {};
};

#endif //laa_state_h
