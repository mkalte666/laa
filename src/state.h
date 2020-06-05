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

/// hardcoded maximum for fft length
static constexpr size_t LAA_MAX_FFT_LENGTH = 131072;
/// hardcoded minimum for fft lenght
static constexpr size_t LAA_MIN_FFT_LENGTH = 1024;
/// hardcoded maximum for fft filtering
static constexpr size_t LAA_MAX_FFT_AVG = 8;

/**
 * \brief Select a window filter that is run over the input
 */
enum class StateWindowFilter {
    None,
    Hamming,
    Blackman
};

/**
 * \brief Filter configuration for State
 *
 * Naming is hard.
 * This is essentially the data that is shared between all states to allow for averaging etc.
 *
 */
struct StateFilterConfig {
    /// ctor
    StateFilterConfig() noexcept;
    /// dtor
    ~StateFilterConfig() noexcept = default;
    /// ctor
    StateFilterConfig(const StateFilterConfig&) noexcept = default;
    /// ctor
    StateFilterConfig(StateFilterConfig&&) noexcept = default;
    /// copy assign
    StateFilterConfig& operator=(const StateFilterConfig&) noexcept = default;
    /// move assign
    StateFilterConfig& operator=(StateFilterConfig&&) noexcept = default;

    /// window filter that is used by state::calc() on input and refernce
    StateWindowFilter windowFilter = StateWindowFilter::Blackman;
    /// the mean of the avgCount past magnitudes
    std::vector<RealVec> avgMagnitudes = {};
    /// number of past states to track
    size_t avgCount = 2;
    /// used by the functions to track the current write position
    size_t currAvg = 0;
    /// used to make sure we scale vectors up/down properly and reset
    size_t lastFftLen = 0;

    /**
     * \brief Calculate the average of the avgCount past magnitudes
     * \param inOut the vector to operate on
     * \param fftLen the number of samples
     */
    void makeAvg(RealVec& inOut, size_t fftLen) noexcept;

    /**
     * \brief Clears all past data (on fftLen changes etc.)
     */
    void clearAvg() noexcept;
};

/**
 * \brief Data of a state
 */
struct StateData {
    /// Number of samples in this state
    size_t fftLen = 0;
    // raw input
    /// Unprocessed input
    RealVec input = {};
    /// Unprocessed reference
    RealVec reference = {};
    // windowed input
    /// input, after the window filter was applied
    RealVec windowedInput = {};
    /// reference, after the window filter was applied
    RealVec windowedReference = {};
    // fft
    /// dft of the input
    ComplexVec fftInput = {};
    /// dft of the reference
    ComplexVec fftReference = {};
    /// averaged magnitude of fftInput
    RealVec avgMag = {};
    /// smoothed average magnitude of fftInput
    RealVec smoothedAvgMag = {};
    // H and h
    /// transfer function
    ComplexVec transferFunction = {};
    /// smoothed transfer function
    ComplexVec smoothedTransferFunction = {};
    /// idft of the tranfer function
    RealVec impulseResponse = {};
    /// smoothed impulseResponse
    RealVec smoothedImpulseResponse = {};
    // coherence and PSD
    /// PSD estimate of the input
    RealVec psdEstimateInput = {};
    /// PSD estimate of the reference
    RealVec psdEstimateReference = {};
    /// Estimate of the csd
    ComplexVec csdEstimate = {};
    /// coherence
    RealVec coherence = {};
    /// smoothed coherence
    RealVec smoothedCoherence = {};

    // this is here for convenience, to be filled in in various places
    /// color to draw this state in. defaults to white
    ImColor uniqueCol = 0xFFFFFFFF; // NOLINT white
    /// name of this state
    std::string name = "";
    /// if this state is active
    bool active = true;
    /// if this state is visible
    bool visible = true;
    /// the amount of time (in seconds) this states fft spans
    double fftDuration = 0.0;
    /// sample rate (in hz) of this state
    double sampleRate = 0.0;
};

/**
 * \brief A state (a processed snapshot in time)
 */
class State {
public:
    /// ctor deleted
    State() = delete;
    /// ctor
    explicit State(size_t fftLen) noexcept;
    /// dtor
    ~State() noexcept;

    /// ctor deleted
    State(const State&) noexcept = delete;
    /// ctor deleted
    State(State&&) noexcept = delete;
    /// assignment deleted
    State& operator=(const State&) noexcept = delete;
    /// assignment deleted
    State& operator=(State&&) noexcept = delete;

    /**
     * \brief Calculate all the things for a state
     * \param filterConfig
     *
     * Takes StateData input and reference.
     * Using those, and filterConfig, calculated all intermediates and results.
     */
    void calc(StateFilterConfig& filterConfig) noexcept;

    /**
     * \brief Return rad-only data (for later copying)
     * \return const ref to data
     */
    const StateData& getData() noexcept;

    /**
     * \brief Access this states data
     * \return ref to data
     */
    StateData& accessData() noexcept;

private:
    /// Data of this state
    StateData data = {};
    /// the fftw plan to calc the dft of the input
    fftw_plan fftInputPlan = {};
    /// the fftw plan to calc the dft of the reference
    fftw_plan fftReferencePlan = {};
    /// the fftw plan to calc the idft of the transfer function
    fftw_plan impulseResponsePlan = {};
};

#endif //laa_state_h
