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
    float h = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 360.0F;
    float s = 90.0F + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 10.0F;
    float v = 50.0F + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 10.0F;
    auto col = ImColor::HSV(h, s, v);
    return col;
}

void StateManager::update(AudioHandler& audioHandler)
{
    if (audioHandler.getFrameCount() > lastFrame) {
        lastFrame = audioHandler.getFrameCount();
        audioHandler.getFrame(liveState.reference, liveState.input);
        hamming(liveState.reference);
        hamming(liveState.input);

        liveState.fftReference = fftReal(liveState.reference);
        liveState.fftInput = fftReal(liveState.input);

        liveState.H.resize(liveState.reference.size());
        liveState.avgH.resize(liveState.reference.size());
        liveState.avgFftReference.resize(liveState.reference.size());
        liveState.avgFftInput.resize(liveState.reference.size());

        mean(liveState.avgFftReference, liveState.fftReference);
        mean(liveState.avgFftInput, liveState.fftInput);

        for (size_t i = 0; i < liveState.avgFftInput.size(); i++) {
            liveState.H[i] = liveState.fftReference[i] / liveState.fftInput[i];
        }
        mean(liveState.avgH, liveState.H);

        liveState.h = ifft(liveState.H);
    }

    ImGui::Begin("Snapshot Control");
    if (ImGui::Button("Capture")) {
        auto copy = liveState;
        copy.uniqueCol = randColor();
        saved.push_back(copy);
    }

    ImGui::ColorButton("Live Data", liveState.uniqueCol);
    ImGui::SameLine();
    ImGui::Checkbox("Show##liveData", &liveState.visible);

    auto iter = saved.begin();
    int c = 0;
    while (iter != saved.end()) {
        c++;
        ImGui::PushID(c);
        ImGui::ColorButton(iter->name.c_str(), iter->uniqueCol);
        ImGui::SameLine();
        ImGui::InputText("##nameInput", &iter->name);
        ImGui::SameLine();
        ImGui::Checkbox("Show##liveData", &iter->visible);
        ImGui::SameLine();
        if (ImGui::Button("x")) {
            iter = saved.erase(iter);
            if (iter == saved.end()) {
                ImGui::PopID();
                break;
            }
        }
        ImGui::PopID();
        iter++;
    }

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
    liveState.uniqueCol = randColor();
    liveState.name = "Live";
}
