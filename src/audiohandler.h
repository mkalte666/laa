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
#include "dsp/sweepgenerator.h"
#include "shared.h"
#include "state.h"

#include <map>
#include <mutex>
#include <queue>
#include <rtaudio/RtAudio.h>
#include <thread>

struct AudioConfig {

    RtAudio::DeviceInfo captureDevice = {};
    RtAudio::DeviceInfo playbackDevice = {};
    unsigned int sampleRate = 48000;
    size_t analysisSamples = 32768;
    double outputVolume = 1.0;
    RtAudio::StreamParameters playbackParams = {};
    RtAudio::StreamParameters captureParams = {};
    unsigned int bufferFrames = 512;
    bool inputAndReferenceAreSwapped = false;
    [[nodiscard]] static std::vector<size_t> getPossibleAnalysisSampleRates() noexcept;
    double samplesToSeconds(size_t count) const noexcept;
    std::string sampleCountToString(size_t count) const noexcept;
    [[nodiscard]] std::vector<unsigned int> getLegalSampleRates() noexcept;
};

enum class FunctionGeneratorType {
    Silence,
    WhiteNoise,
    PinkNoise,
    Sine,
    Sweep
};
std::string getStr(const FunctionGeneratorType& gen) noexcept;

class AudioHandler {
public:
    AudioHandler() noexcept;
    AudioHandler(const AudioHandler&) = delete;
    AudioHandler(AudioHandler&&) = delete;
    AudioHandler& operator=(const AudioHandler&) = delete;
    AudioHandler& operator=(AudioHandler&&) = delete;
    ~AudioHandler() noexcept;

    void update() noexcept;

    size_t getFrameCount() const noexcept;
    StateData getStateData() const noexcept;

    const AudioConfig& getConfig() const noexcept;

private:
    double genNextPlaybackSample();

    void startAudio();
    void stopAudio();
    void playbackCallback(void* stream, size_t len);
    void captureCallback(void* stream, size_t len);
    static int rtAudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nFrames, double, RtAudioStreamStatus, void* userData);
    void resetStates() noexcept;

    AudioConfig config = {};
    std::unique_ptr<RtAudio> rtAudio = {};
    std::string status = "Not Started";
    bool running = false;

    PinkNoiseGenerator pinkNoise = {};
    SineGenerator sineGenerator = {};
    SweepGenerator sweepGenerator = {};
    FunctionGeneratorType functionGeneratorType = FunctionGeneratorType::Silence;

    void processingWorker() noexcept;
    using StatePoolArray = std::array<State*, 5>;
    std::map<size_t, StatePoolArray> statePool = {};
    std::thread dataProcessor = {};
    bool terminateThreads = false;
    mutable std::mutex callbackLock = {};
    mutable std::mutex processingLock = {};
    std::queue<State*> unusedStates = {};
    size_t sampleCount = 0;
    State* captureState = nullptr;
    std::queue<State*> processStates = {};
    State* doneState = nullptr;
    size_t frameCount = 0;

    StateFilterConfig stateFilterConfig = {};
};

#endif //laa_audiohandler_h
