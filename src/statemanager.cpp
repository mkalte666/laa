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

#include "statemanager.h"
#include <random>

ImColor randColor()
{
    static std::vector<ImColor> colors = {};

    if (colors.empty()) {
        colors.emplace_back(255, 0, 0, 255);
        colors.emplace_back(0, 255, 0, 255);
        colors.emplace_back(0, 0, 255, 255);
        colors.emplace_back(255, 0, 255, 255);
        colors.emplace_back(0, 255, 255, 255);
        colors.emplace_back(255, 255, 0, 255);
        colors.emplace_back(125, 0, 0, 125);
        colors.emplace_back(0, 125, 0, 125);
        colors.emplace_back(0, 0, 125, 125);
        colors.emplace_back(125, 0, 125, 125);
        colors.emplace_back(0, 125, 125, 125);
        colors.emplace_back(125, 125, 0, 125);
    }

    auto col = colors.back();
    colors.pop_back();
    return col;
}

void StateManager::update(AudioHandler& audioHandler)
{
    if (audioHandler.getFrameCount() > lastFrame) {
        lastFrame = audioHandler.getFrameCount();
        bool configAvgResetNeede = liveState.config.analysisSamples != audioHandler.getConfig().analysisSamples;
        liveState.config = audioHandler.getConfig();

        audioHandler.getFrame(liveState.reference, liveState.input);

        liveState.windowedReference = liveState.reference;
        blackman(liveState.windowedReference);
        liveState.windowedInput = liveState.input;
        blackman(liveState.windowedInput);

        liveState.fftReference = fftReal(liveState.windowedReference);
        liveState.fftInput = fftReal(liveState.windowedInput);
        liveState.polarFftInput = liveState.fftInput;
        toPolar(liveState.polarFftInput);

        if (liveState.avgCount == 0) {
            liveState.avgFftReference = liveState.fftReference;
            liveState.avgFftInput = liveState.fftInput;
        } else {
            if (liveState.pastFftInput.size() != liveState.avgCount || liveState.pastFftReference.size() != liveState.avgCount || configAvgResetNeede) {
                resetAvg();
            }

            for (size_t i = 1; i < liveState.avgCount; i++) {
                liveState.pastFftInput[i - 1] = std::move(liveState.pastFftInput[i]);
                liveState.pastFftReference[i - 1] = std::move(liveState.pastFftReference[i]);
            }
            liveState.pastFftReference.back() = liveState.fftReference;
            liveState.pastFftInput.back() = liveState.fftInput;

            liveState.avgFftReference.resize(liveState.fftReference.size(), 0.0);
            liveState.avgFftInput.resize(liveState.fftInput.size(), 0.0);
            double dCount = static_cast<double>(liveState.avgCount);
            for (size_t i = 0; i < liveState.avgFftReference.size(); i++) {
                for (size_t iAvg = 0; iAvg < liveState.avgCount; iAvg++) {
                    liveState.avgFftReference[i] += liveState.pastFftReference[iAvg][i];
                    liveState.avgFftInput[i] += liveState.pastFftInput[iAvg][i];
                }
                liveState.avgFftReference[i] /= dCount;
                liveState.avgFftInput[i] /= dCount;
            }
        }

        liveState.avgPolarFftInput = liveState.avgFftInput;
        toPolar(liveState.avgPolarFftInput);

        liveState.frequencyResponse.resize(liveState.input.size());
        liveState.avgFrequencyResponse.resize(liveState.input.size());
        for (size_t i = 0; i < liveState.fftInput.size(); i++) {
            liveState.frequencyResponse[i] = liveState.fftInput[i] / liveState.fftReference[i];
            liveState.avgFrequencyResponse[i] = liveState.avgFftInput[i] / liveState.avgFftReference[i];
            //liveState.frequencyResponse[i] = liveState.fftReference[i] / liveState.fftInput[i];
            //liveState.avgFrequencyResponse[i] = liveState.avgFftReference[i] / liveState.avgFftInput[i];
        }

        liveState.impulseResponse = ifft(liveState.frequencyResponse);
        liveState.avgImpluseResponse = ifft(liveState.avgFrequencyResponse);
    }

    ImGui::Begin("Snapshot Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);
    ImGui::PushItemWidth(-1.0F);
    if (ImGui::Button("Capture")) {
        auto copy = liveState;
        copy.uniqueCol = randColor();
        copy.active = false;
        saved.push_back(copy);
    }

    if (ImGui::RadioButton("##liveRadio", liveState.active)) {
        deactivateAll();
        liveState.active = true;
    }
    ImGui::SameLine();
    ImGui::ColorButton("Live Data", liveState.uniqueCol);
    ImGui::SameLine();
    ImGui::Checkbox("Live Data##liveData", &liveState.visible);

    auto iter = saved.begin();
    int c = 0;
    while (iter != saved.end()) {
        c++;
        ImGui::PushID(c);
        if (ImGui::RadioButton("##savedRadio", iter->active)) {
            deactivateAll();
            iter->active = true;
        }
        ImGui::SameLine();
        ImGui::ColorButton(iter->name.c_str(), iter->uniqueCol);
        ImGui::SameLine();
        ImGui::Checkbox("##ShowliveData", &iter->visible);
        ImGui::SameLine();
        if (ImGui::Button("x")) {
            iter = saved.erase(iter);
            if (iter == saved.end()) {
                ImGui::PopID();
                break;
            }
        }
        ImGui::SameLine();
        ImGui::InputText("##nameInput", &iter->name);
        ImGui::PopID();
        iter++;
    }

    ImGui::PopItemWidth();
    ImGui::End();
}

const State& StateManager::getLive() const noexcept
{
    return liveState;
}

const std::list<State>& StateManager::getSaved() const noexcept
{
    return saved;
}

StateManager::StateManager() noexcept
{
    liveState.name = "Live";
    liveState.active = true;
}

void StateManager::deactivateAll()
{
    liveState.active = false;
    for (auto& state : saved) {
        state.active = false;
    }
}
void StateManager::resetAvg()
{
    liveState.pastFftReference.clear();
    liveState.pastFftInput.clear();

    liveState.pastFftReference.resize(liveState.avgCount);
    liveState.pastFftInput.resize(liveState.avgCount);

    for (size_t i = 0; i < liveState.avgCount; i++) {
        liveState.pastFftReference[i].resize(liveState.fftReference.size(), 0.0);
        liveState.pastFftInput[i].resize(liveState.fftInput.size(), 0.0);
    }
}
