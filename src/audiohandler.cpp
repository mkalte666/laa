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

std::string getStr(const StateWindowFilter& filter) noexcept
{
    switch (filter) {
    case StateWindowFilter::None:
        return "None";
    case StateWindowFilter::Hamming:
        return "Hamming";
    case StateWindowFilter::Blackman:
        return "Blackman";
    }

    return "";
}

AudioHandler::AudioHandler() noexcept
{
    rtAudio = std::make_unique<RtAudio>();
    if (rtAudio == nullptr) {
        std::cout << "Could not start audio driver";
        SDL2WRAP_ASSERT(false);
    }
    if (auto deviceId = rtAudio->getDefaultOutputDevice(); deviceId != 0) {
        config.device = rtAudio->getDeviceInfo(deviceId);
    }

    config.playbackParams.nChannels = 2;
    config.playbackParams.firstChannel = 0;
    config.captureParams.nChannels = 2;
    config.captureParams.firstChannel = 0;

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
    for (auto rate : config.getPossibleAnalysisSampleRates()) {
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

void AudioHandler::update() noexcept
{
    ImGui::Begin("Audio Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysVerticalScrollbar);
    ImGui::PushItemWidth(-1.0F);
    if (!running) {

        ImGui::Text("Driver");
        std::vector<RtAudio::Api> rtAudioApis;
        RtAudio::getCompiledApi(rtAudioApis);
        if (ImGui::BeginCombo("##apiSelect", RtAudio::getApiDisplayName(rtAudio->getCurrentApi()).c_str())) {
            for (auto& api : rtAudioApis) {
                if (ImGui::Selectable(RtAudio::getApiDisplayName(api).c_str(), api == rtAudio->getCurrentApi())) {
                    rtAudio = std::make_unique<RtAudio>(api);
                    if (rtAudio == nullptr) {
                        rtAudio = std::make_unique<RtAudio>();
                        if (rtAudio == nullptr) {
                            SDL2WRAP_ASSERT(false);
                        }
                    }
                    break;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::Text("Device ");
        if (ImGui::BeginCombo("##Device", config.device.name.c_str())) {
            for (unsigned int i = 0; i < rtAudio->getDeviceCount(); i++) {
                auto device = rtAudio->getDeviceInfo(i);
                if (device.duplexChannels < 2) {
                    continue;
                }
                ImGui::PushID(static_cast<int>(i));
                if (ImGui::Selectable(device.name.c_str(), device.name == config.device.name)) {
                    config.device = device;
                    config.sampleRate = config.device.preferredSampleRate;
                    config.captureParams.deviceId = i;
                    config.playbackParams.deviceId = i;
                }
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Sample Rate");
        if (ImGui::BeginCombo("##Sample Rate", std::to_string(config.sampleRate).c_str())) {
            for (auto rate : config.device.sampleRates) {
                ImGui::PushID(static_cast<int>(rate));
                if (ImGui::Selectable(std::to_string(rate).c_str(), rate == config.sampleRate)) {
                    config.sampleRate = rate;
                }
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }

    } else {
        ImGui::Text("Device: %s", config.device.name.c_str());
        ImGui::Text("Sample Rate: %d", static_cast<int>(config.sampleRate));
    }

    if (!running) {
        if (ImGui::Button("Start Audio")) {
            startAudio();
        }
    } else {
        if (ImGui::Button("Stop Audio")) {
            stopAudio();
            status = "Stopped";
        }
    }

    ImGui::Text("Status: %s", status.c_str());

    ImGui::Separator();

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
    ImGui::Text("Window Filter");
    if (ImGui::BeginCombo("##Window Config", getStr(stateFilterConfig.windowFilter).c_str())) {
        if (ImGui::Selectable("None", stateFilterConfig.windowFilter == StateWindowFilter::None)) {
            stateFilterConfig.windowFilter = StateWindowFilter::None;
        }
        if (ImGui::Selectable("Hamming", stateFilterConfig.windowFilter == StateWindowFilter::Hamming)) {
            stateFilterConfig.windowFilter = StateWindowFilter::Hamming;
        }
        if (ImGui::Selectable("Blackman", stateFilterConfig.windowFilter == StateWindowFilter::Blackman)) {
            stateFilterConfig.windowFilter = StateWindowFilter::Blackman;
        }
        ImGui::EndCombo();
    }
    ImGui::Text("FFT Averaging");
    auto iAvgCount = static_cast<int>(stateFilterConfig.avgCount);
    ImGui::InputInt("##avgCount", &iAvgCount, 1, LAA_MAX_FFT_AVG);
    stateFilterConfig.avgCount = std::clamp(static_cast<size_t>(iAvgCount), static_cast<size_t>(0), LAA_MAX_FFT_LENGTH);
    if (ImGui::Button("Reset Avg")) {
        stateFilterConfig.clearAvg();
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
    stopAudio();
    rtAudio->openStream(&config.captureParams, &config.playbackParams, RTAUDIO_FLOAT32, config.sampleRate, &config.bufferFrames, &rtAudioCallback, this);
    rtAudio->startStream();

    running = true;
    status = std::string("Running (") + s2::Audio::getCurrentDriver() + ")";
}

void AudioHandler::stopAudio()
{
    if (running) {
        resetStates();
        rtAudio->stopStream();
    }

    running = false;
}

void AudioHandler::playbackCallback(void* stream, size_t len)
{
    auto count = static_cast<size_t>(len) / sizeof(float);
    auto* ptr = reinterpret_cast<float*>(stream);
    for (auto i = 0ull; i + 1 < count; i += 2) {
        auto f = config.outputVolume * genNextPlaybackSample();

        ptr[i] = static_cast<float>(f);
        ptr[i + 1] = static_cast<float>(f);
    }
}

void AudioHandler::captureCallback(void* stream, size_t len)
{
    auto count = static_cast<size_t>(len) / sizeof(float);
    auto* ptr = reinterpret_cast<float*>(stream);
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

    for (auto i = 0ull; i + 1 < count; i += 2) {
        auto reference = ptr[i + 0];
        auto input = ptr[i + 1];
        auto dReference = static_cast<double>(reference);
        auto dInput = static_cast<double>(input);

        captureState->accessData().reference[sampleCount] = dReference;
        captureState->accessData().input[sampleCount] = dInput;
        ++sampleCount;

        if (sampleCount >= config.analysisSamples) {
            callbackLock.lock();
            ++frameCount;
            sampleCount = 0;
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

int AudioHandler::rtAudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nFrames, double, RtAudioStreamStatus, void* userData)
{
    auto* handler = reinterpret_cast<AudioHandler*>(userData);
    handler->captureCallback(inputBuffer, nFrames * 2 * sizeof(float));
    handler->playbackCallback(outputBuffer, nFrames * 2 * sizeof(float));
    return 0;
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

StateData AudioHandler::getStateData() const noexcept
{
    StateData copy = {};
    if (doneState == nullptr) {
        return copy;
    }

    processingLock.lock();
    callbackLock.lock();

    copy = doneState->getData();

    callbackLock.unlock();
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
        callbackLock.lock();
        if (!processStates.empty()) {
            current = processStates.front();
            processStates.pop();
        }
        callbackLock.unlock();

        if (current == nullptr) {
            continue;
        }

        // this takes time, and is the reason we are a thread
        current->calc(stateFilterConfig);

        // advance the doneState
        processingLock.lock();
        if (doneState != nullptr) {
            callbackLock.lock();
            unusedStates.push(doneState);
            callbackLock.unlock();
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
