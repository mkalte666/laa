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

#ifndef laa_audioconfig_h
#define laa_audioconfig_h

#include "../shared.h"
#include "../state/state.h"

#include <rtaudio/RtAudio.h>

/**
 * \brief Configuration for the audio capture and playback.
 */
struct AudioConfig {
    /// Default Sample Rate
    static constexpr unsigned int defaultSampleRate = 48000;
    /// Default Analysis Sample Count
    static constexpr size_t defaultAnalysisSamples = 32768;
    /// Default size of the Buffer for the callbacks
    static constexpr int defaultBufferFrames = 512;

    /// RTAudio Capture Device
    RtAudio::DeviceInfo captureDevice = {};
    /// RTAudio Playback Device
    RtAudio::DeviceInfo playbackDevice = {};

    /// Sample Rate
    unsigned int sampleRate = defaultSampleRate;
    /// Analysis Sampe Count
    size_t analysisSamples = defaultAnalysisSamples;
    /// Output Volume
    double outputVolume = 1.0;

    /// Parameters for the Playback
    RtAudio::StreamParameters playbackParams = {};
    /// Parameters for the Capture
    RtAudio::StreamParameters captureParams = {};
    /// Buffer of bytes for the callback
    unsigned int bufferFrames = defaultBufferFrames;
    /// True if output and reference are swapped on the input side
    bool inputAndReferenceAreSwapped = false;

    /**
     * \brief Return the number of possible fft lengths
     * \return A vector containing possible fft lengths
     */
    [[nodiscard]] static std::vector<size_t> getPossibleAnalysisSampleRates() noexcept;

    /**
     * \brief Convert a number of samples to the time in seconds
     * \param count sample count to convert to seconds
     * \return count in seconds
     */
    [[nodiscard]] double samplesToSeconds(size_t count) const noexcept;

    /**
     * \brief Same as \sa samplesToSeconds, but returns a string instead
     * \param count sample count to convert to seconds
     * \return count in seconds, but as string
     */
    [[nodiscard]] std::string sampleCountToString(size_t count) const noexcept;

    /**
     * \brief Return Sample rates supported by both the input and the output device
     * \return A vector containing the sample rates
     */
    [[nodiscard]] std::vector<unsigned int> getLegalSampleRates() noexcept;
};

#endif //laa_audioconfig_h
