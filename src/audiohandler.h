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

#ifndef laa_audiohandler_h
#define laa_audiohandler_h

#include "dsp/pinknoisegenerator.h"
#include "dsp/sinegenerator.h"
#include "shared.h"

struct AudioConfig {
    std::string driver = "";
    std::string captureName = "None";
    std::string playbackName = "None";
    size_t referenceChannel = 0;
    size_t inputChannel = 1;
    size_t sampleRate = 48000;
    size_t samples = 4096;
    size_t analysisSamples = 32768;

    std::vector<size_t> getPossibleAnalysisSampleRates() const noexcept;
    double samplesToSeconds(size_t count) const noexcept;
    std::string sampleCountToString(size_t count) const noexcept;
};

enum class FunctionGeneratorType {
    Silence,
    WhiteNoise,
    PinkNoise,
    Sine
};
std::string getStr(const FunctionGeneratorType& gen) noexcept;

class AudioHandler {
public:
    AudioHandler() noexcept;
    AudioHandler(const AudioHandler&) = delete;
    AudioHandler(AudioHandler&&) = default;
    AudioHandler& operator=(const AudioHandler&) = delete;
    AudioHandler& operator=(AudioHandler&&) = default;
    ~AudioHandler() noexcept;

    void update() noexcept;

    size_t getFrameCount() const noexcept;
    void getFrame(std::vector<double>& reference, std::vector<double>& input) const noexcept;

    const AudioConfig& getConfig() const noexcept;

private:
    double genNextPlaybackSample();

    void startAudio();
    void playbackCallback(Uint8* stream, int len);
    void captureCallback(Uint8* stream, int len);
    static void playbackCallbackStatic(void* userdata, Uint8* stream, int len);
    static void captureCallbackStatic(void* userdata, Uint8* stream, int len);

    AudioConfig config = {};
    bool driverChosen = false;
    std::string status = "Not Started";
    bool running = false;
    s2::Audio::DeviceID captureId = {};
    s2::Audio::DeviceID playbackId = {};
    PinkNoiseGenerator pinkNoise = {};
    SineGenerator sineGenerator = {};
    FunctionGeneratorType functionGeneratorType = FunctionGeneratorType::Silence;

    std::vector<double> currentReferenceSignal = {};
    std::vector<double> currentInputSignal = {};
    std::vector<double> wipReferenceSignal = {};
    std::vector<double> wipInputSignal = {};
    size_t frameCount = 0;
};

#endif //laa_audiohandler_h
