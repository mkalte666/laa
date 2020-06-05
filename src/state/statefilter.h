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

#ifndef laa_statefilter_h
#define laa_statefilter_h

#include "../shared.h"

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
    void filter(RealVec& inOut, size_t fftLen) noexcept;

    /**
     * \brief Clears all past data (on fftLen changes etc.)
     */
    void clearAvg() noexcept;
};

#endif //laa_statefilter_h
