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

#include "audiohandler.h"

#include <filesystem> // exsists
namespace fs = std::filesystem;

void clearStateQueue(std::queue<State*>& q)
{
    std::queue<State*> empty;
    std::swap(q, empty);
}

AudioHandler::AudioHandler() noexcept
{
    rtAudio = std::make_unique<RtAudio>();
    if (rtAudio == nullptr) {
        std::cout << "Could not start audio driver";
        SDL2WRAP_ASSERT(false);
    }

    // some defaults
    config.captureDevice = rtAudio->getDeviceInfo(rtAudio->getDefaultInputDevice());
    config.playbackDevice = rtAudio->getDeviceInfo(rtAudio->getDefaultOutputDevice());
    config.playbackParams.deviceId = rtAudio->getDefaultOutputDevice();
    config.captureParams.deviceId = rtAudio->getDefaultInputDevice();
    config.playbackParams.nChannels = 2;
    config.playbackParams.firstChannel = 0;
    config.captureParams.nChannels = 2;
    config.captureParams.firstChannel = 0;

    // check if we have wisdom available
    // wisdom is this magic "resource" coming from fftw
    // essentially it saves what it knows about the most performant way to calc to the drive
    // we ask sdl for the path. i didnt ad that function to sdl2wrap yet, so yeah
    auto* pWisdomPath = SDL_GetPrefPath("mkalte", "laa");
    std::string wisdomPath = pWisdomPath;
    SDL_free(pWisdomPath); // yep

    wisdomPath += "/fftwWisdom" + getVersionString() + ".fftw";
    // we can only import wisdom if we exsist
    if (fs::exists(wisdomPath)) {
        fftw_import_wisdom_from_filename(wisdomPath.c_str());
    }

    // populate state pool
    // due to the nature for fftw, this might take a while...
    // state creation creates the fftw things!
    for (auto rate : config.getPossibleAnalysisSampleRates()) {
        StatePoolArray pool;
        for (auto& state : pool) {
            state = new State(rate);
        }
        statePool[rate] = pool;
    }

    // save wisdom. see above
    fftw_export_wisdom_to_filename(wisdomPath.c_str());

    // reset everything
    resetStates();

    // spin up data processing thread
    dataProcessor = std::thread([this]() {
        this->processingWorker();
    });

    // and done!
}

AudioHandler::~AudioHandler() noexcept
{
    if (running) {
        stopAudio();
        rtAudio.reset();
    }

    // destroy thread
    terminateThreads = true;
    dataProcessor.join();

    // clean up states
    for (auto& pair : statePool) {
        for (auto state : pair.second) {
            delete state;
        }
    }
}

void AudioHandler::startAudio()
{
    // make sure the generators have the right rate
    sineGenerator.setSampleRate(config.sampleRate);
    sweepGenerator.setSampleRate(config.sampleRate);
    sweepGenerator.setLength(static_cast<double>(config.analysisSamples) / config.sampleRate);

    // assure we are not running anymore
    stopAudio();

    // opens the streams. this throws if there is an error. let it crash for now.
    rtAudio->openStream(&config.playbackParams, &config.captureParams, RTAUDIO_FLOAT32, config.sampleRate, &config.bufferFrames, &rtAudioCallback, this);
    rtAudio->startStream();

    running = true;
    status = std::string("Running (") + s2::Audio::getCurrentDriver() + ")";
}

void AudioHandler::stopAudio()
{
    // just kill of rtaudio
    if (running) {
        resetStates();
        rtAudio->stopStream();
        rtAudio->closeStream();
    }

    running = false;
}

void AudioHandler::resetStates() noexcept
{
    // halt the audio world
    processingLock.lock();
    callbackLock.lock();

    // clear them all
    doneState = nullptr;
    captureState = nullptr;
    clearStateQueue(unusedStates);
    clearStateQueue(processStates);

    // fill in the proper ones
    for (auto& state : statePool[config.analysisSamples]) {
        unusedStates.push(state);
    }

    // can run again
    callbackLock.unlock();
    processingLock.unlock();
}

size_t AudioHandler::getFrameCount() const noexcept
{
    return frameCount;
}

const AudioConfig& AudioHandler::getConfig() const noexcept
{
    return config;
}
