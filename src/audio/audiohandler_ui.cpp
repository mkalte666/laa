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

#include "../midpointslider.h"
#include "audiohandler.h"

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

    case FunctionGeneratorType::Sweep:
        return "Sweep";
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

static std::string ApiName(const RtAudio::Api AApi)
{
#if defined(RTAUDIO500)
    switch (AApi) {
    case RtAudio::UNSPECIFIED: /*!< Search for a working compiled API. */
        return "Unspecified";
    case RtAudio::LINUX_ALSA: /*!< The Advanced Linux Sound Architecture API. */
        return "ALSA";
    case RtAudio::LINUX_PULSE: /*!< The Linux PulseAudio API. */
        return "Pulse";
    case RtAudio::LINUX_OSS: /*!< The Linux Open Sound System API. */
        return "OSS";
    case RtAudio::UNIX_JACK: /*!< The Jack Low-Latency Audio Server API. */
        return "JACK";
    case RtAudio::MACOSX_CORE: /*!< Macintosh OS-X Core Audio API. */
        return "Core";
    case RtAudio::WINDOWS_WASAPI: /*!< The Microsoft WASAPI API. */
        return "WASAPI";
    case RtAudio::WINDOWS_ASIO: /*!< The Steinberg Audio Stream I/O API. */
        return "ASIO";
    case RtAudio::WINDOWS_DS: /*!< The Microsoft Direct Sound API. */
        return "DirectSound";
    case RtAudio::RTAUDIO_DUMMY: /*!< A compilable but non-functional API. */
        return "Dummy";
    default:
        return "(INVALID)";
    }
#else
    return RtAudio::getApiDisplayName(AApi);
#endif
}

void AudioHandler::update() noexcept
{
    ImGui::Begin("Audio Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysVerticalScrollbar);
    ImGui::PushItemWidth(-1.0F);
    if (!running) {
        ImGui::TextWrapped("Driver");
        std::vector<RtAudio::Api> rtAudioApis;
        RtAudio::getCompiledApi(rtAudioApis);
        if (ImGui::BeginCombo("##apiSelect", ApiName(rtAudio->getCurrentApi()).c_str())) {
            for (auto& api : rtAudioApis) {
                if (ImGui::Selectable(ApiName(api).c_str(), api == rtAudio->getCurrentApi())) {
                    rtAudio = std::make_unique<RtAudio>(api);
                    if (rtAudio == nullptr) {
                        rtAudio = std::make_unique<RtAudio>();
                        if (rtAudio == nullptr) {
                            SDL_assert_always(false);
                        }
                    }
                    config.captureDevice = rtAudio->getDeviceInfo(rtAudio->getDefaultInputDevice());
                    config.playbackDevice = rtAudio->getDeviceInfo(rtAudio->getDefaultOutputDevice());
                    config.playbackParams.deviceId = rtAudio->getDefaultOutputDevice();
                    config.captureParams.deviceId = rtAudio->getDefaultInputDevice();
                    break;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::TextWrapped("Capture Device");
        if (ImGui::BeginCombo("##CapDevice", config.captureDevice.name.c_str())) {
            for (unsigned int i = 0; i < rtAudio->getDeviceCount(); i++) {
                auto device = rtAudio->getDeviceInfo(i);
                if (device.inputChannels < 2) {
                    continue;
                }
                ImGui::PushID(static_cast<int>(i));
                if (ImGui::Selectable(device.name.c_str(), device.name == config.captureDevice.name)) {
                    config.captureDevice = device;
                    config.sampleRate = config.captureDevice.preferredSampleRate;
                    config.captureParams.deviceId = i;
                }
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }
        ImGui::TextWrapped("Playback Device");
        if (ImGui::BeginCombo("##PbDevice", config.playbackDevice.name.c_str())) {
            for (unsigned int i = 0; i < rtAudio->getDeviceCount(); i++) {
                auto device = rtAudio->getDeviceInfo(i);
                if (device.outputChannels < 2) {
                    continue;
                }
                ImGui::PushID(static_cast<int>(i));
                if (ImGui::Selectable(device.name.c_str(), device.name == config.playbackDevice.name)) {
                    config.playbackDevice = device;
                    config.playbackParams.deviceId = i;
                }
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }

        ImGui::TextWrapped("Sample Rate");
        if (ImGui::BeginCombo("##Sample Rate", std::to_string(config.sampleRate).c_str())) {
            for (auto rate : config.getLegalSampleRates()) {
                ImGui::PushID(static_cast<int>(rate));
                if (ImGui::Selectable(std::to_string(rate).c_str(), rate == config.sampleRate)) {
                    config.sampleRate = rate;
                    // make the generators have the right rate
                    sineGenerator.setSampleRate(config.sampleRate);
                    sweepGenerator.setSampleRate(config.sampleRate);
                }
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }

        int iFirstPlayback = static_cast<int>(config.playbackParams.firstChannel);
        int iFirstCapture = static_cast<int>(config.captureParams.firstChannel);
        ImGui::TextWrapped("First Capture");
        ImGui::InputInt("##firstCap", &iFirstCapture, 1, 1);
        ImGui::Checkbox("Swap Reference", &config.inputAndReferenceAreSwapped);
        ImGui::TextWrapped("First Playback");
        ImGui::InputInt("##firstPlayback", &iFirstPlayback, 1, 1);

        bool enableInternal = config.channelCount == 1;
        ImGui::Checkbox("Internal Reference", &enableInternal);
        config.channelCount = enableInternal ? 1 : 2; // 2 channels with reference, one without
        config.playbackParams.firstChannel = std::clamp(static_cast<unsigned int>(iFirstPlayback), 0u, config.playbackDevice.outputChannels - config.channelCount);
        config.captureParams.firstChannel = std::clamp(static_cast<unsigned int>(iFirstCapture), 0u, config.captureDevice.inputChannels - config.channelCount);
    } else {
        ImGui::TextWrapped("Capture Device: %s", config.captureDevice.name.c_str());
        ImGui::TextWrapped("Playback Device: %s", config.playbackDevice.name.c_str());
        ImGui::TextWrapped("Sample Rate: %d", static_cast<int>(config.sampleRate));
        ImGui::TextWrapped("First Playback: %d", static_cast<int>(config.playbackParams.firstChannel));
        ImGui::TextWrapped("First Capture: %d", static_cast<int>(config.playbackParams.firstChannel));
        ImGui::TextWrapped("Internal Reference: %s", config.channelCount == 2 ? "no" : "yes");
        if (config.inputAndReferenceAreSwapped) {
            ImGui::TextWrapped("Input and Ref Swapped!");
        }
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

    ImGui::TextWrapped("Status: %s", status.c_str());

    ImGui::Separator();

    ImGui::TextWrapped("Select Signal");
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
        if (ImGui::Selectable(getStr(FunctionGeneratorType::Sweep).c_str(), functionGeneratorType == FunctionGeneratorType::Sweep)) {
            functionGeneratorType = FunctionGeneratorType::Sweep;
        }
        ImGui::EndCombo();
    }
    if (functionGeneratorType == FunctionGeneratorType::Sweep && stateFilterConfig.windowFilter != StateWindowFilter::None) {
        ImGui::TextWrapped("Disable Window Filter for Sweep!");
    }

    if (functionGeneratorType == FunctionGeneratorType::Sine) {
        auto freq = static_cast<float>(sineGenerator.getFrequency());
        ImGui::TextWrapped("Frequency");
        if (ImGui::SliderFloat("##Frequency", &freq, 0.0F, 20000.0F, "%.0f", 1.0F)) {
            sineGenerator.setFrequency(static_cast<double>(freq));
        }
    }
    ImGui::TextWrapped("Output Volume");
    MidpointSlider("##volime", 0.0, 1.0, 0.5, config.outputVolume);

    ImGui::Separator();
    ImGui::TextWrapped("Analysis Length");
    if (ImGui::BeginCombo("##Analysis Length", config.sampleCountToString(config.analysisSamples).c_str())) {
        for (auto&& rate : config.getPossibleAnalysisSampleRates()) {
            ImGui::PushID(static_cast<int>(rate));
            if (ImGui::Selectable(config.sampleCountToString(rate).c_str(), rate == config.analysisSamples)) {
                config.analysisSamples = rate;
                sweepGenerator.setLength(static_cast<double>(config.analysisSamples) / config.sampleRate);
                resetStates();
            }
            ImGui::PopID();
        }

        ImGui::EndCombo();
    }
    ImGui::TextWrapped("Window Filter");
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
    ImGui::TextWrapped("FFT Averaging");
    auto iAvgCount = static_cast<int>(stateFilterConfig.avgCount);
    ImGui::InputInt("##avgCount", &iAvgCount, 1, 1);
    stateFilterConfig.avgCount = std::clamp(static_cast<size_t>(iAvgCount), static_cast<size_t>(0), LAA_MAX_FFT_AVG);
    if (ImGui::Button("Reset Avg")) {
        stateFilterConfig.clearAvg();
    }

    ImGui::PopItemWidth();
    ImGui::End();
}