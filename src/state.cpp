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

State::State(size_t fftLen) noexcept
{
    data.uniqueCol = ImGui::GetColorU32(ImGuiCol_Text);
    data.fftLen = std::min(LAA_MAX_FFT_LENGTH, std::max(LAA_MIN_FFT_LENGTH, fftLen));
    data.input.resize(LAA_MAX_FFT_LENGTH);
    data.reference.resize(LAA_MAX_FFT_LENGTH);
    data.windowedInput.resize(LAA_MAX_FFT_LENGTH);
    data.windowedReference.resize(LAA_MAX_FFT_LENGTH);
    data.fftInput.resize(LAA_MAX_FFT_LENGTH);
    data.fftReference.resize(LAA_MAX_FFT_LENGTH);
    data.avgMag.resize(LAA_MAX_FFT_LENGTH);
    data.smoothedAvgMag.resize(LAA_MAX_FFT_LENGTH);
    data.transferFunction.resize(LAA_MAX_FFT_LENGTH);
    data.smoothedTransferFunction.resize(LAA_MAX_FFT_LENGTH);
    data.impulseResponse.resize(LAA_MAX_FFT_LENGTH);
    data.smoothedImpulseResponse.resize(LAA_MAX_FFT_LENGTH);
    data.psdEstimateInput.resize(LAA_MAX_FFT_LENGTH);
    data.psdEstimateReference.resize(LAA_MAX_FFT_LENGTH);
    data.csdEstimate.resize(LAA_MAX_FFT_LENGTH);
    data.coherence.resize(LAA_MAX_FFT_LENGTH);
    data.smoothedCoherence.resize(LAA_MAX_FFT_LENGTH);

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

    // filter magnitude
    filterConfig.makeAvg(data.avgMag, data.fftLen);

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
    // normalize
    for (size_t i = 0; i < data.fftLen; i++) {
        data.impulseResponse[i] /= dFftLen;
    }
    // smooth out things
    smooth(data.smoothedAvgMag, data.avgMag);
    smooth(data.smoothedTransferFunction, data.transferFunction);
    smooth(data.smoothedImpulseResponse, data.impulseResponse);
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

StateFilterConfig::StateFilterConfig() noexcept
{
    avgMagnitudes.resize(LAA_MAX_FFT_AVG, RealVec());
    for (size_t i = 0; i < LAA_MAX_FFT_AVG; i++) {
        avgMagnitudes[i].resize(LAA_MAX_FFT_LENGTH);
    }
}
void StateFilterConfig::clearAvg() noexcept
{
    for (size_t i = 0; i < LAA_MAX_FFT_AVG; i++) {
        for (size_t j = 0; j < LAA_MAX_FFT_LENGTH; j++) {
            avgMagnitudes[i][j] = 0.0;
        }
    }
}

void StateFilterConfig::makeAvg(RealVec& inOut, size_t fftLen) noexcept
{
    if (avgCount == 0) {
        return;
    }

    if (fftLen != lastFftLen) {
        clearAvg();
        lastFftLen = fftLen;
    }

    for (size_t i = 0; i < fftLen; i++) {
        avgMagnitudes[currAvg][i] = inOut[i];
        inOut[i] = 0.0;
        for (size_t avgI = 0; avgI < avgCount; avgI++) {
            inOut[i] += avgMagnitudes[avgI][i];
        }
        inOut[i] /= static_cast<double>(avgCount);
    }

    ++currAvg;
    if (currAvg >= avgCount) {
        currAvg = 0;
    }
}
