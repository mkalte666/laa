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
static constexpr size_t LAA_MAX_FFT_AVG = 8;

enum class StateWindowFilter {
    None,
    Hamming,
    Blackman
};

struct StateFilterConfig {
    StateFilterConfig() noexcept;
    ~StateFilterConfig() noexcept = default;
    StateFilterConfig(const StateFilterConfig&) noexcept = default;
    StateFilterConfig(StateFilterConfig&&) noexcept = default;
    StateFilterConfig& operator=(const StateFilterConfig&) noexcept = default;
    StateFilterConfig& operator=(StateFilterConfig&&) noexcept = default;

    StateWindowFilter windowFilter = StateWindowFilter::Blackman;
    std::vector<RealVec> avgMagnitudes = {};
    size_t avgCount = 2;
    size_t currAvg = 0;
    size_t lastFftLen = 0;
    void makeAvg(RealVec& inOut, size_t fftLen) noexcept;
    void clearAvg() noexcept;
};

struct StateData {
    size_t fftLen = 0;
    // raw input
    RealVec input = {};
    RealVec reference = {};
    // windowed input
    RealVec windowedInput = {};
    RealVec windowedReference = {};
    // fft
    ComplexVec fftInput = {};
    ComplexVec fftReference = {};
    RealVec avgMag = {};
    RealVec smoothedAvgMag = {};
    // H and h
    ComplexVec transferFunction = {};
    ComplexVec smoothedTransferFunction = {};
    RealVec impulseResponse = {};
    RealVec smoothedImpulseResponse = {};
    // coherence and PSD
    RealVec psdEstimateInput = {};
    RealVec psdEstimateReference = {};
    ComplexVec csdEstimate = {};
    RealVec coherence = {};
    RealVec smoothedCoherence = {};

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

    void calc(StateFilterConfig& filterConfig) noexcept;

    const StateData& getData() noexcept;
    StateData& accessData() noexcept;

private:
    StateData data = {};
    fftw_plan fftInputPlan = {};
    fftw_plan fftReferencePlan = {};
    fftw_plan impulseResponsePlan = {};
};

#endif //laa_state_h
