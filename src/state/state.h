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

#include "shared.h"
#include "statedata.h"
#include "statefilter.h"

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
