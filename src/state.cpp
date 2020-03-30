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

#include "state.h"

State::State(size_t fftLen) noexcept
{
    data.fftLen = std::min(LAA_MAX_FFT_LENGTH, std::max(LAA_MIN_FFT_LENGTH, fftLen));
    data.input.resize(LAA_MAX_FFT_LENGTH);
    data.reference.resize(LAA_MAX_FFT_LENGTH);
    data.windowedInput.resize(LAA_MAX_FFT_LENGTH);
    data.windowedReference.resize(LAA_MAX_FFT_LENGTH);
    data.fftInput.resize(LAA_MAX_FFT_LENGTH);
    data.fftReference.resize(LAA_MAX_FFT_LENGTH);
    data.filteredFftInput.resize(LAA_MAX_FFT_LENGTH);
    data.filteredFftReference.resize(LAA_MAX_FFT_LENGTH);
    data.filteredMagFftInput.resize(LAA_MAX_FFT_LENGTH);
    data.phaseDelta.resize(LAA_MAX_FFT_LENGTH);
    data.frequencyResponse.resize(LAA_MAX_FFT_LENGTH);
    data.impulseResponse.resize(LAA_MAX_FFT_LENGTH);

    fftInputPlan = fftw_plan_dft_r2c_1d(static_cast<int>(data.fftLen), reinterpret_cast<double*>(data.windowedInput.data()), reinterpret_cast<fftw_complex*>(data.fftInput.data()), FFTW_MEASURE);
    fftReferencePlan = fftw_plan_dft_r2c_1d(static_cast<int>(data.fftLen), reinterpret_cast<double*>(data.windowedReference.data()), reinterpret_cast<fftw_complex*>(data.fftReference.data()), FFTW_MEASURE);
    impulseResponsePlan = fftw_plan_dft_c2r_1d(static_cast<int>(data.fftLen), reinterpret_cast<fftw_complex*>(data.frequencyResponse.data()), reinterpret_cast<double*>(data.impulseResponse.data()), FFTW_MEASURE);
}

State::~State() noexcept
{
    fftw_destroy_plan(impulseResponsePlan);
    fftw_destroy_plan(fftReferencePlan);
    fftw_destroy_plan(fftInputPlan);
}

void State::calc() noexcept
{
    // copy input into windows
    blackman(data.windowedInput, data.input);
    blackman(data.windowedReference, data.reference);

    // run fft for input and reference
    fftw_execute(fftInputPlan);
    fftw_execute(fftReferencePlan);

    // FIXME: filtering is disabled right now
    for (size_t i = 0; i < data.fftLen; i++) {
        data.filteredFftInput[i] = data.fftInput[i];
        data.filteredFftReference[i] = data.fftReference[i];
    }

    // make polar representations and frequency response
    for (size_t i = 0; i < data.fftLen; i++) {
        // polar and phase delta
        data.filteredMagFftInput[i] = mag(data.filteredFftInput[i]);
        data.phaseDelta[i] = phase(data.filteredFftInput[i]) - phase(data.filteredFftReference[i]);

        // frequency response
        data.frequencyResponse[i] = data.filteredFftInput[i] / data.filteredFftReference[i];
    }

    // compute impulse response
    fftw_execute(impulseResponsePlan);
    // thise one is unnormalited however
    auto dFftLen = static_cast<double>(data.fftLen);
    for (size_t i = 0; i < data.fftLen; ++i) {
        data.impulseResponse[i] /= dFftLen;
    }
}

const StateData& State::getData() noexcept
{
    return data;
}

StateData& State::accessData() noexcept
{
    return data;
}
