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

// just switches between audio sources for playback
double AudioHandler::genNextPlaybackSample()
{
    switch (functionGeneratorType) {
    case FunctionGeneratorType::Silence:
        break;
    case FunctionGeneratorType::WhiteNoise:
        return (WhiteNoiseGenerator::nextSample());
    case FunctionGeneratorType::PinkNoise:
        return (pinkNoise.nextSample());
    case FunctionGeneratorType::Sine:
        return (sineGenerator.nextSample());
    case FunctionGeneratorType::Sweep:
        return sweepGenerator.nextSample();
    }

    return 0.0F;
}

// RT Audio does one callback call every time both the input and the output buffer are full
// playbackCallback and captureCallback are seperate but are both fed from here
int AudioHandler::rtAudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nFrames, double, RtAudioStreamStatus, void* userData)
{
    // callback data is this, so NOLINTNEXTLINE
    auto* handler = reinterpret_cast<AudioHandler*>(userData);
    handler->captureCallback(inputBuffer, nFrames * 2 * sizeof(float));
    handler->playbackCallback(outputBuffer, nFrames * 2 * sizeof(float));

    // why wouldn't we succeed?
    return 0;
}

// fill the output buffer with bytes
void AudioHandler::playbackCallback(void* stream, size_t len)
{
    // this, combined with the rt audio callback, is a bit awkward but i have not had the time to clean it up yet
    auto count = static_cast<size_t>(len) / sizeof(float);
    // void pointers do that. NOLINTNEXTLINE
    auto* ptr = reinterpret_cast<float*>(stream);

    for (auto i = 0ULL; i + 1 < count; i += 2) {
        // next sample scaled by the output volume. Nothing to see here really
        auto f = config.outputVolume * genNextPlaybackSample();

        // id like to do this without pointer wush, but whatever
        ptr[i] = static_cast<float>(f); //NOLINT
        ptr[i + 1] = static_cast<float>(f); //NOLINT
    }
}

void AudioHandler::captureCallback(void* stream, size_t len)
{
    // this, combined with the rt audio callback, is a bit awkward but i have not had the time to clean it up yet
    auto count = static_cast<size_t>(len) / sizeof(float);
    // void pointers do that. NOLINTNEXTLINE
    auto* ptr = reinterpret_cast<float*>(stream);

    // first check if there is no current capture State.
    // We then try to get one from the unusedState queue.
    // if there is none, we cant do things.
    if (captureState == nullptr) {
        callbackLock.lock();
        if (unusedStates.empty()) {
            callbackLock.unlock();
            return;
        }

        captureState = unusedStates.front();
        unusedStates.pop();
        callbackLock.unlock();
    }

    // then we loop over samples.
    for (auto i = 0ULL; i + 1 < count; i += 2) {
        // samples are coming in as flaot32, but the stream is a raw pointer.
        auto reference = ptr[i + (config.inputAndReferenceAreSwapped ? 1 : 0)]; // NOLINT
        auto input = ptr[i + (config.inputAndReferenceAreSwapped ? 0 : 1)]; // NOLINT
        // and convert to double, as we wanna process stuff as double
        auto dReference = static_cast<double>(reference);
        auto dInput = static_cast<double>(input);

        // we put the samples back at the end of our current state
        captureState->accessData().reference[sampleCount] = dReference;
        captureState->accessData().input[sampleCount] = dInput;
        ++sampleCount;

        // and if the current state is full, we put it into processStates.
        // we do not yet increase framecount - thats done by the audio processing thread.
        // also, as we are already locking anyway, try to set up the next captureState. same as at the top, really
        if (sampleCount >= config.analysisSamples) {
            callbackLock.lock();

            sampleCount = 0; // dont forget this!
            processStates.push(captureState);
            captureState = nullptr;

            if (unusedStates.empty()) {
                callbackLock.unlock();
                return;
            }
            captureState = unusedStates.front();
            unusedStates.pop();
            callbackLock.unlock();
        }
    }
}

// processes audio samples. What this really means is, get them form the queue and call calc
void AudioHandler::processingWorker() noexcept
{
    // needed for sleep
    using namespace std::chrono;

    // terminateThreads is called in the dtor of AudioHandler and kills us of.
    while (!terminateThreads) {
        // sleep (yield?) so that we dont eat all the cpu time when idle
        std::this_thread::sleep_for(5ms);

        // current is our current audio state.
        // lock, see if there is something in the queue.
        State* current = nullptr;
        callbackLock.lock();
        if (!processStates.empty()) {
            current = processStates.front();
            processStates.pop();
        }
        callbackLock.unlock();

        // if there was nothing, we got nothing to do
        if (current == nullptr) {
            continue;
        }

        // this takes time, and is the reason we are a thread
        current->calc(stateFilterConfig);

        // advance the doneState
        // we give the current state back to the unused queue, to be picked back up by the audio capture.
        processingLock.lock();
        if (doneState != nullptr) {
            callbackLock.lock();
            unusedStates.push(doneState);
            callbackLock.unlock();
        }
        doneState = current;
        ++frameCount; // here we finally increase the frame count - just after updating the done state.
        processingLock.unlock();
    }
}

// return a copy of the state data
StateData AudioHandler::getStateData() const noexcept
{
    // first check if there is any. if not, nothing to do
    StateData copy = {};
    if (doneState == nullptr) {
        return copy;
    }

    // only the processor touches the done state
    // the processor only locks the callbacks if it can lock this lock
    // so no need to do anything special, the processor can wait for the copy
    processingLock.lock();
    copy = doneState->getData();
    processingLock.unlock();

    // copy some config infos over into the state
    copy.sampleRate = static_cast<double>(config.sampleRate);
    copy.fftDuration = config.samplesToSeconds(config.analysisSamples);
    return copy;
}
