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
#include "dsp/smoothing.h"
#include "dsp/windows.h"

State::State(size_t fftLen) noexcept
{
    data.uniqueCol = ImGui::GetColorU32(ImGuiCol_Text);
    data.fftLen = std::min(LAA_MAX_FFT_LENGTH, std::max(LAA_MIN_FFT_LENGTH, fftLen));
    data.input.resize(data.fftLen);
    data.reference.resize(data.fftLen);
    data.windowedInput.resize(data.fftLen);
    data.windowedReference.resize(data.fftLen);
    data.fftInput.resize(data.fftLen);
    data.fftReference.resize(data.fftLen);
    data.avgMag.resize(data.fftLen);
    data.smoothedAvgMag.resize(data.fftLen);
    data.transferFunction.resize(data.fftLen);
    data.smoothedTransferFunction.resize(data.fftLen);
    data.impulseResponse.resize(data.fftLen);
    data.smoothedImpulseResponse.resize(data.fftLen);
    data.psdEstimateInput.resize(data.fftLen);
    data.psdEstimateReference.resize(data.fftLen);
    data.csdEstimate.resize(data.fftLen);
    data.coherence.resize(data.fftLen);
    data.smoothedCoherence.resize(data.fftLen);

    fftInputPlan = fftw_plan_dft_r2c_1d(static_cast<int>(data.fftLen), reinterpret_cast<double*>(data.windowedInput.data()), reinterpret_cast<fftw_complex*>(data.fftInput.data()), FFTW_MEASURE);
    fftReferencePlan = fftw_plan_dft_r2c_1d(static_cast<int>(data.fftLen), reinterpret_cast<double*>(data.windowedReference.data()), reinterpret_cast<fftw_complex*>(data.fftReference.data()), FFTW_MEASURE);
    impulseResponsePlan = fftw_plan_dft_c2r_1d(static_cast<int>(data.fftLen), reinterpret_cast<fftw_complex*>(data.transferFunction.data()), reinterpret_cast<double*>(data.impulseResponse.data()), FFTW_MEASURE | FFTW_PRESERVE_INPUT);
}

State::~State() noexcept
{
    fftw_destroy_plan(impulseResponsePlan);
    fftw_destroy_plan(fftReferencePlan);
    fftw_destroy_plan(fftInputPlan);
}

void State::calc(StateFilterConfig& filterConfig) noexcept
{
    // copy input into windows
    switch (filterConfig.windowFilter) {

    case StateWindowFilter::None:
        noWindow(data.windowedInput, data.input);
        noWindow(data.windowedReference, data.reference);
        break;
    case StateWindowFilter::Hamming:
        hamming(data.windowedInput, data.input);
        hamming(data.windowedReference, data.reference);
        break;
    case StateWindowFilter::Blackman:
        blackman(data.windowedInput, data.input);
        blackman(data.windowedReference, data.reference);
        break;
    }

    // run fft for input and reference
    fftw_execute(fftInputPlan);
    fftw_execute(fftReferencePlan);

    // make things we can derive from the fft
    auto dFftLen = static_cast<double>(data.fftLen);
    for (size_t i = 0; i < data.fftLen; i++) {
        // normalize first
        data.fftInput[i] /= dFftLen;
        data.fftReference[i] /= dFftLen;
        // magnitude into avgMag
        data.avgMag[i] = mag(data.fftInput[i]);
        // transfer function:  XxH = Y => H = Y/X
        data.transferFunction[i] = data.fftInput[i] / data.fftReference[i];
    }

    // divide our range into segments
    // estimate psd and csd over these segments
    // then estimate the squared coherence at a point.
    size_t psdDepth = std::clamp(data.fftLen / 1024ull, 64ull, 512ull);
    for (size_t i = 0; i < data.fftLen; i++) {
        size_t start = i < psdDepth ? 0 : i - psdDepth;
        size_t end = std::min(data.fftLen, i + psdDepth);
        data.psdEstimateInput[i] = 0.0;
        data.psdEstimateReference[i] = 0.0;
        data.csdEstimate[i] = 0.0;
        for (size_t j = start; j < end; j++) {
            data.psdEstimateReference[i] += magSquared(data.fftReference[j]);
            data.psdEstimateInput[i] += magSquared(data.fftInput[j]);
            data.csdEstimate[i] += conj(data.fftReference[j]) * data.fftInput[j];
        }
        data.coherence[i] = magSquared(data.csdEstimate[i]) / (data.psdEstimateReference[i] * data.psdEstimateInput[i]);
    }

    // compute impulse response
    fftw_execute(impulseResponsePlan);
    // normalize and mean of ir
    double meanIr = 0.0;
    double varIr = 0.0;
    for (size_t i = 0; i < data.fftLen; i++) {
        data.impulseResponse[i] /= dFftLen;
        // while we are at it, also build up meanIr
        meanIr += data.impulseResponse[i];
    }
    meanIr /= dFftLen;
    // variance of ir
    for (size_t i = 0; i < data.fftLen; i++) {
        varIr += (data.impulseResponse[i] - meanIr) * (data.impulseResponse[i] - meanIr);
    }
    varIr /= dFftLen;
    // now that we know the variance, we can cut off things in the ir that are not significant
    for (size_t i = 0; i < data.fftLen; i++) {
        // we dont care about anything within std deviation
        double distSquare = std::pow(data.impulseResponse[i] - meanIr, 2.0);
        if (distSquare < varIr) {
            data.smoothedImpulseResponse[i] = 0.0;
            continue;
        }

        data.smoothedImpulseResponse[i] = data.impulseResponse[i];
    }

    // filters
    filterConfig.filter(data.avgMag, data.fftLen);

    // smooth out things
    smooth(data.smoothedAvgMag, data.avgMag);
    smooth(data.smoothedTransferFunction, data.transferFunction);
    //smooth(data.smoothedImpulseResponse, data.impulseResponse);
    smooth(data.smoothedCoherence, data.coherence);
}

const StateData& State::getData() noexcept
{
    return data;
}

StateData& State::accessData() noexcept
{
    return data;
}
