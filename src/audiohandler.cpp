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

AudioHandler::AudioHandler() noexcept
{
    if (s2::Audio::getNumDevices(false) > 0) {
        config.playbackName = s2::Audio::getDeviceName(0, false);
    }

    if (s2::Audio::getNumDevices(true) > 0) {
        config.captureName = s2::Audio::getDeviceName(0, true);
    }
}

AudioHandler::~AudioHandler() noexcept
{
    if (running) {
        s2::Audio::closeDevice(captureId);
        s2::Audio::closeDevice(playbackId);
    }
}

void AudioHandler::update() noexcept
{
    ImGui::Begin("Audio Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::BeginCombo("Playback Device", config.playbackName.c_str())) {
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

    if (ImGui::BeginCombo("Capture Device", config.captureName.c_str())) {
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

    ImGui::InputInt("Sample Rate", &config.sampleRate);
    if (!running) {
        if (ImGui::Button("Start Audio")) {
            startAudio();
        }
    } else {
        if (ImGui::Button("Restart Audio")) {
            status = "Restarting is not implemented yet";
        }
    }

    ImGui::Text("Status: %s", status.c_str());
    ImGui::End();
}

void AudioHandler::startAudio()
{
    if (running) {
        s2::Audio::closeDevice(captureId);
        s2::Audio::closeDevice(playbackId);
    }

    running = false;

    s2::Audio::Spec want = {};
    want.freq = config.sampleRate;
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = config.samples;

    auto wantPlayback = want;
    wantPlayback.callback = playbackCallbackStatic;
    auto wantCapture = want;
    wantCapture.callback = captureCallbackStatic;

    s2::Audio::Spec gotPlayback;
    auto playbackRes = s2::Audio::openDevice(config.playbackName.c_str(), false, wantPlayback, gotPlayback, s2::AudioAllow::AnyChange);
    if (!playbackRes) {
        status = playbackRes.getError().msg;
        return;
    }
    playbackId = playbackRes.extractValue();

    s2::Audio::Spec gotCapture;
    auto captureRes = s2::Audio::openDevice(config.playbackName.c_str(), false, wantCapture, gotCapture, s2::AudioAllow::AnyChange);
    if (!captureRes) {
        status = captureRes.getError().msg;
        s2::Audio::closeDevice(playbackId);
        return;
    }
    captureId = captureRes.extractValue();

    running = true;
    status = "Running";
}

void AudioHandler::playbackCallback(Uint8* stream, int len)
{
    auto count = static_cast<size_t>(len) / sizeof(float);
    auto* floatPtr = reinterpret_cast<float*>(stream);
    for (auto i = 0ull; i < count; i++) {
        floatPtr[i] = 0.0F;
    }
}

void AudioHandler::captureCallback(Uint8* stream, int len)
{
    (void)stream;
    (void)len;
    //auto count = static_cast<size_t>(len) / sizeof(float);
    //auto* floatPtr = reinterpret_cast<float*>(stream);
}

void AudioHandler::playbackCallbackStatic(void* userdata, Uint8* stream, int len)
{
    reinterpret_cast<AudioHandler*>(userdata)->playbackCallback(stream, len);
}

void AudioHandler::captureCallbackStatic(void* userdata, Uint8* stream, int len)
{
    reinterpret_cast<AudioHandler*>(userdata)->captureCallback(stream, len);
}
