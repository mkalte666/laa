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

#ifndef laa_statedata_h
#define laa_statedata_h

#include "../shared.h"

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

#endif //laa_statedata_h
