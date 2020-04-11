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
#include "midpointslider.h"
#include <filesystem>

namespace fs = std::filesystem;

void clearStateQueue(std::queue<State*>& q)
{
    std::queue<State*> empty;
    std::swap(q, empty);
}

std::string getStr(const FunctionGeneratorType& gen) noexcept
{
    switch (gen) {
    case FunctionGeneratorType::Silence:
        return "Silence";

    case FunctionGeneratorType::WhiteNoise:
        return "White Noise";

    case FunctionGeneratorType::PinkNoise:
        return "Pink Noise";

    case FunctionGeneratorType::Sine:
        return "Sine";
    }

    return "";
}

AudioHandler::AudioHandler() noexcept
{
    if (s2::Audio::getNumDevices(false) > 0) {
        config.playbackName = s2::Audio::getDeviceName(0, false);
    }

    if (s2::Audio::getNumDevices(true) > 0) {
        config.captureName = s2::Audio::getDeviceName(0, true);
    }

    if (s2::Audio::getNumDrivers() > 0) {
        config.driver = s2::Audio::getDriver(0);
    }

    auto res = s2::Audio::init(config.driver.c_str());
    if (!res) {
        SDL2WRAP_ASSERT(false);
    }
    driverChosen = true;

    // check if we have wisdom available
    auto pWisdomPath = SDL_GetPrefPath("mkalte", "laa");
    std::string wisdomPath = pWisdomPath;
    SDL_free(pWisdomPath);
    wisdomPath += "/fftwWisdom" + getVersionString() + ".fftw";
    if (fs::exists(wisdomPath)) {
        fftw_import_wisdom_from_filename(wisdomPath.c_str());
    }
    // populate state pool
    // due to the nature for fftw, this might take a while...
    for (auto rate : AudioConfig::getPossibleAnalysisSampleRates()) {
        StatePoolArray pool;
        for (auto& state : pool) {
            state = new State(rate);
        }
        statePool[rate] = pool;
    }

    // save wisdom
    fftw_export_wisdom_to_filename(wisdomPath.c_str());

    resetStates();

    // spin up data processing thread
    dataProcessor = std::thread([this]() {
        this->processingWorker();
    });
}

AudioHandler::~AudioHandler() noexcept
{
    if (running) {
        s2::Audio::closeDevice(captureId);
        s2::Audio::closeDevice(playbackId);
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

void AudioHandler::update() noexcept
{
    ImGui::Begin("Audio Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);
    ImGui::PushItemWidth(-1.0F);
    if (!running) {
        ImGui::Text("Driver");
        if (ImGui::BeginCombo("##Driver", config.driver.c_str())) {
            for (int i = 0; i < s2::Audio::getNumDrivers(); i++) {
                ImGui::PushID(i);
                if (ImGui::Selectable(s2::Audio::getDriver(i))) {
                    s2::Audio::quit();
                    config.driver = s2::Audio::getDriver(i);
                    auto res = s2::Audio::init(config.driver.c_str());
                    SDL2WRAP_ASSERT(res.hasValue());
                }
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Playback Device");
        if (ImGui::BeginCombo("##Playback Device", config.playbackName.c_str())) {
            for (int i = 0; i < s2::Audio::getNumDevices(false); i++) {
                std::string playbackName = s2::Audio::getDeviceName(i, false);
                ImGui::PushID(i);
                if (ImGui::Selectable(playbackName.c_str(), playbackName == config.playbackName)) {
                    config.playbackName = playbackName;
                }
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Capture Device");
        if (ImGui::BeginCombo("##Capture Device", config.captureName.c_str())) {
            for (int i = 0; i < s2::Audio::getNumDevices(true); i++) {
                std::string captureName = s2::Audio::getDeviceName(i, true);
                ImGui::PushID(i);
                if (ImGui::Selectable(captureName.c_str(), captureName == config.captureName)) {
                    config.captureName = captureName;
                }
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Sample Rate");
        if (ImGui::BeginCombo("##Sample Rate", std::to_string(config.sampleRate).c_str())) {
            if (ImGui::Selectable("44100", config.sampleRate == 44100)) {
                config.sampleRate = 44100;
            }
            if (ImGui::Selectable("48000", config.sampleRate == 48000)) {
                config.sampleRate = 48000;
            }
            if (ImGui::Selectable("96000", config.sampleRate == 96000)) {
                config.sampleRate = 96000;
            }
            //if (ImGui::Selectable("192000", config.sampleRate == 192000)) {
            //    config.sampleRate = 192000;
            //}
            ImGui::EndCombo();
        }
    } else {
        ImGui::Text("Driver: %s", config.driver.c_str());
        ImGui::Text("Playback: %s", config.playbackName.c_str());
        ImGui::Text("Capture: %s", config.captureName.c_str());
        ImGui::Text("Sample Rate: %d", static_cast<int>(config.sampleRate));
    }

    if (!running) {
        if (ImGui::Button("Start Audio")) {
            startAudio();
        }
    } else {
        if (ImGui::Button("Stop Audio")) {
            status = "Stopping is not implemented yet";
        }
    }

    ImGui::Text("Status: %s", status.c_str());

    ImGui::Separator();

    int refChan = static_cast<int>(config.referenceChannel);
    int inputChan = static_cast<int>(config.inputChannel);

    ImGui::Text("Reference Channel");
    ImGui::InputInt("##refChan", &refChan, 1, 1);
    refChan = std::clamp(refChan, 0, 1);
    ImGui::Text("Input Channel");
    ImGui::InputInt("##inputChan", &inputChan, 1, 1);
    inputChan = std::clamp(inputChan, 0, 1);

    config.referenceChannel = static_cast<size_t>(refChan);
    config.inputChannel = static_cast<size_t>(inputChan);

    ImGui::Separator();

    ImGui::Text("Select Signal");
    if (ImGui::BeginCombo("##Select Signal", getStr(functionGeneratorType).c_str())) {
        if (ImGui::Selectable(getStr(FunctionGeneratorType::Silence).c_str(), functionGeneratorType == FunctionGeneratorType::Silence)) {
            functionGeneratorType = FunctionGeneratorType::Silence;
        }
        if (ImGui::Selectable(getStr(FunctionGeneratorType::Sine).c_str(), functionGeneratorType == FunctionGeneratorType::Sine)) {
            functionGeneratorType = FunctionGeneratorType::Sine;
        }
        if (ImGui::Selectable(getStr(FunctionGeneratorType::WhiteNoise).c_str(), functionGeneratorType == FunctionGeneratorType::WhiteNoise)) {
            functionGeneratorType = FunctionGeneratorType::WhiteNoise;
        }
        if (ImGui::Selectable(getStr(FunctionGeneratorType::PinkNoise).c_str(), functionGeneratorType == FunctionGeneratorType::PinkNoise)) {
            functionGeneratorType = FunctionGeneratorType::PinkNoise;
        }

        ImGui::EndCombo();
    }

    if (functionGeneratorType == FunctionGeneratorType::Sine) {
        auto freq = static_cast<float>(sineGenerator.getFrequency());
        ImGui::Text("Frequency");
        if (ImGui::SliderFloat("##Frequency", &freq, 0.0F, 20000.0F, "%.0f", 1.0F)) {
            sineGenerator.setFrequency(static_cast<double>(freq));
        }
    }
    ImGui::Text("Output Volume");
    MidpointSlider("##volime", 0.0, 1.0, 0.5, config.outputVolume);

    ImGui::Separator();
    ImGui::Text("Analysis Length");
    if (ImGui::BeginCombo("##Analysis Length", config.sampleCountToString(config.analysisSamples).c_str())) {
        for (auto&& rate : config.getPossibleAnalysisSampleRates()) {
            ImGui::PushID(static_cast<int>(rate));
            if (ImGui::Selectable(config.sampleCountToString(rate).c_str(), rate == config.analysisSamples)) {
                config.analysisSamples = rate;
                resetStates();
            }
            ImGui::PopID();
        }

        ImGui::EndCombo();
    }

    ImGui::PopItemWidth();
    ImGui::End();
}

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
        break;
    }

    return 0.0F;
}

void AudioHandler::startAudio()
{
    if (running) {
        s2::Audio::closeDevice(captureId);
        s2::Audio::closeDevice(playbackId);
    }

    running = false;

    s2::Audio::Spec want;
    SDL_zero(want);
    want.freq = static_cast<int>(config.sampleRate);
    want.format = AUDIO_S32SYS;
    want.channels = 2;
    want.samples = static_cast<Uint16>(config.samples);
    want.userdata = this;

    auto wantPlayback = want;
    wantPlayback.callback = playbackCallbackStatic;
    auto wantCapture = want;
    wantCapture.callback = captureCallbackStatic;

    s2::Audio::Spec gotPlayback;
    auto playbackRes = s2::Audio::openDevice(config.playbackName.c_str(), false, wantPlayback, gotPlayback, static_cast<s2::AudioAllow>(0));
    if (!playbackRes) {
        status = playbackRes.getError().msg;
        return;
    }
    playbackId = playbackRes.extractValue();
    s2::Audio::pauseDevice(playbackId, false);

    s2::Audio::Spec gotCapture;
    auto captureRes = s2::Audio::openDevice(config.captureName.c_str(), true, wantCapture, gotCapture, static_cast<s2::AudioAllow>(0));
    if (!captureRes) {
        status = captureRes.getError().msg;
        s2::Audio::closeDevice(playbackId);
        return;
    }
    captureId = captureRes.extractValue();
    s2::Audio::pauseDevice(captureId, false);

    running = true;
    status = std::string("Running (") + s2::Audio::getCurrentDriver() + ")";
}

void AudioHandler::playbackCallback(Uint8* stream, int len)
{
    auto count = static_cast<size_t>(len) / sizeof(Sint32);
    auto* ptr = reinterpret_cast<Sint32*>(stream);
    for (auto i = 0ull; i + 1 < count; i += 2) {
        double f = config.outputVolume * genNextPlaybackSample() * SDL_MAX_SINT32;

        ptr[i] = static_cast<Sint32>(f);
        ptr[i + 1] = static_cast<Sint32>(f);
    }
}

void AudioHandler::captureCallback(Uint8* stream, int len)
{
    auto count = static_cast<size_t>(len) / sizeof(Sint32);
    auto* ptr = reinterpret_cast<Sint32*>(stream);
    if (captureState == nullptr) {
        if (unusedStates.empty()) {
            return;
        }

        captureState = unusedStates.front();
        unusedStates.pop();
    }

    for (auto i = 0ull; i + 1 < count; i += 2) {
        auto reference = ptr[i + config.referenceChannel];
        auto input = ptr[i + config.inputChannel];
        auto dReference = static_cast<double>(reference) / static_cast<double>(SDL_MAX_SINT32);
        auto dInput = static_cast<double>(input) / static_cast<double>(SDL_MAX_SINT32);

        captureState->accessData().reference[sampleCount] = dReference;
        captureState->accessData().input[sampleCount] = dInput;
        ++sampleCount;

        if (sampleCount >= config.analysisSamples) {
            ++frameCount;
            sampleCount = 0;
            processStates.push(captureState);
            captureState = nullptr;

            if (unusedStates.empty()) {
                return;
            }
            captureState = unusedStates.front();
            unusedStates.pop();
        }
    }
}

void AudioHandler::playbackCallbackStatic(void* userdata, Uint8* stream, int len)
{
    reinterpret_cast<AudioHandler*>(userdata)->playbackCallback(stream, len);
}

void AudioHandler::captureCallbackStatic(void* userdata, Uint8* stream, int len)
{
    reinterpret_cast<AudioHandler*>(userdata)->captureCallback(stream, len);
}

void AudioHandler::resetStates() noexcept
{
    // halt the audio world
    processingLock.lock();
    s2::Audio::lockDevice(captureId);
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
    s2::Audio::unlockDevice(captureId);
    processingLock.unlock();
}

size_t AudioHandler::getFrameCount() const noexcept
{
    return frameCount;
}

StateData AudioHandler::getStateData() const noexcept
{
    StateData copy = {};
    if (doneState == nullptr) {
        return copy;
    }

    processingLock.lock();
    s2::Audio::lockDevice(captureId);

    copy = doneState->getData();

    s2::Audio::unlockDevice(captureId);
    processingLock.unlock();

    copy.sampleRate = static_cast<double>(config.sampleRate);
    copy.fftDuration = config.samplesToSeconds(config.analysisSamples);
    return copy;
}

const AudioConfig& AudioHandler::getConfig() const noexcept
{
    return config;
}

void AudioHandler::processingWorker() noexcept
{
    using namespace std::chrono;
    while (!terminateThreads) {
        std::this_thread::sleep_for(5ms);
        State* current = nullptr;
        s2::Audio::lockDevice(captureId);
        if (!processStates.empty()) {
            current = processStates.front();
            processStates.pop();
        }
        s2::Audio::unlockDevice(captureId);

        if (current == nullptr) {
            continue;
        }

        // this takes time, and is the reason we are a thread
        current->calc();

        // advance the doneState
        processingLock.lock();
        if (doneState != nullptr) {
            s2::Audio::lockDevice(captureId);
            unusedStates.push(doneState);
            s2::Audio::unlockDevice(captureId);
        }
        doneState = current;
        processingLock.unlock();
    }
}

std::vector<size_t> AudioConfig::getPossibleAnalysisSampleRates() noexcept
{
    std::vector<size_t> rates;
    for (size_t i = LAA_MIN_FFT_LENGTH; i < LAA_MAX_FFT_LENGTH; i <<= 1u) {
        rates.push_back(i);
    }

    return rates;
}

double AudioConfig::samplesToSeconds(size_t count) const noexcept
{
    return static_cast<double>(count) / static_cast<double>(sampleRate);
}

std::string AudioConfig::sampleCountToString(size_t count) const noexcept
{
    std::string result;
    double seconds = samplesToSeconds(count);
    result = std::to_string(count) + " (" + std::to_string(seconds) + "s / " + std::to_string(1.0 / seconds) + "Hz)";
    return result;
}
