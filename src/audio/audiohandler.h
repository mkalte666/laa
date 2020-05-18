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

#include "../dsp/pinknoisegenerator.h"
#include "../dsp/sinegenerator.h"
#include "../dsp/sweepgenerator.h"
#include "audioconfig.h"

#include <map>
#include <mutex>
#include <queue>
#include <thread>

/**
 * \brief Enumerize the dsp sample generators
 */
enum class FunctionGeneratorType {
    Silence,
    WhiteNoise,
    PinkNoise,
    Sine,
    Sweep
};

/**
 * \brief Convert the FunctionGeneratorType enum to a string
 * \param gen FunctionGeneratorType to stringify
 * \return gen as a string
 */
std::string getStr(const FunctionGeneratorType& gen) noexcept;

/**
 * \brief Handles audio and audio UI config
 */
class AudioHandler {
public:
    /// ctor
    AudioHandler() noexcept;
    /// deleted
    AudioHandler(const AudioHandler&) = delete;
    /// deleted
    AudioHandler(AudioHandler&&) = delete;
    /// deleted
    AudioHandler& operator=(const AudioHandler&) = delete;
    /// deleted
    AudioHandler& operator=(AudioHandler&&) = delete;
    /// deleted
    ~AudioHandler() noexcept;

    /**
     * \brief Update UI.
     */
    void update() noexcept;

    /**
     * \brief Gen number of frames
     * \note track the result and use it to check if there are any new frames around
     * \return
     */
    size_t getFrameCount() const noexcept;

    /**
     * \brief Get a copy of the data of the current state
     * \return StateData of the current state
     */
    StateData getStateData() const noexcept;

    /**
     * \brief Return a const ref to the current configuration
     * \return Current audio config
     */
    const AudioConfig& getConfig() const noexcept;

private:
    /**
     * \brief Generates the next playback sample for output
     * \return
     */
    double genNextPlaybackSample();

    /**
     * \brief Starts the audio backend and spins up callbacks
     */
    void startAudio();
    /**
     * \brief Kills of the audio backend
     */
    void stopAudio();

    /**
     * \brief The member portion of the audio playback callback
     * \param stream
     * \param len
     */
    void playbackCallback(void* stream, size_t len);

    /**
     * \brief The member portion of the audio capture callback
     * \param stream
     * \param len
     */
    void captureCallback(void* stream, size_t len);

    /**
     * \brief The static callback called by rtaudio internals
     * \param outputBuffer
     * \param inputBuffer
     * \param nFrames
     * \param userData
     * \return
     */
    static int rtAudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nFrames, double, RtAudioStreamStatus, void* userData);

    /**
     * \brief Reset the sates
     */
    void resetStates() noexcept;

    /// current audio config
    AudioConfig config = {};
    /// rt audio instance
    std::unique_ptr<RtAudio> rtAudio = {};
    /// status string display
    std::string status = "Not Started";
    /// true if audio is running, false if not
    bool running = false;

    /// generates pink noise
    PinkNoiseGenerator pinkNoise = {};
    /// generates a sine
    SineGenerator sineGenerator = {};
    /// generates a sweep
    SweepGenerator sweepGenerator = {};
    /// switches between audio generators.
    FunctionGeneratorType functionGeneratorType = FunctionGeneratorType::Silence;

    /// thread worker for audio processing
    void processingWorker() noexcept;
    /// std::threa for the processingWorker
    std::thread dataProcessor = {};
    /// helps killing off the processing thread
    bool terminateThreads = false;
    /// protects the audio queue
    mutable std::mutex callbackLock = {};
    /// protects the processing queue
    mutable std::mutex processingLock = {};

    /// use shared pointers so we have less of a foot gun
    using StatePtr = std::shared_ptr<State>;
    /// pool of audio states (stateData + fluff around it)
    using StatePoolArray = std::array<StatePtr, 5>;
    /// map of states. map key is the analysis lengths
    std::map<size_t, StatePoolArray> statePool = {};

    /// states current available for processing
    std::queue<StatePtr> unusedStates = {};

    /// state that is current captured into
    StatePtr captureState = nullptr;
    /// number of samples already inside captureState
    size_t sampleCount = 0;
    /// states ready for processing
    std::queue<StatePtr> processStates = {};
    /// the state that is done with processing and can be used
    StatePtr doneState = nullptr;
    /// counts up every time a state is done with processing
    size_t frameCount = 0;

    /// configuration of the audio filter - shared between all states
    StateFilterConfig stateFilterConfig = {};
};

#endif //laa_audiohandler_h
