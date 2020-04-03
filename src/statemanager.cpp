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
        for (int i = 0; i < 15; i++) {
            float f = static_cast<float>(i) / 15.0F;
            float f2 = 1.0F - (0.3F * f);
            colors.emplace_back(ImColor::HSV(f, f2, 1.0));
        }
    }

    auto col = colors.back();
    colors.pop_back();
    return col;
}

void StateManager::update(AudioHandler& audioHandler)
{
    if (audioHandler.getFrameCount() > lastFrame) {
        liveState = audioHandler.getStateData();
    }

    ImGui::Begin("Snapshot Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);
    ImGui::PushItemWidth(-1.0F);

#ifdef LAA_GL_ES_2
    size_t maxCaptures = 4;
#else
    size_t maxCaptures = 10;
#endif
    if (saved.size() < maxCaptures && ImGui::Button("Capture")) {
        auto copy = liveState;
        copy.uniqueCol = randColor();
        copy.active = false;
        copy.visible = liveVisible;
        saved.push_back(copy);
    }

    if (ImGui::RadioButton("##liveRadio", liveActive)) {
        deactivateAll();
        liveActive = true;
    }
    ImGui::SameLine();
    ImGui::ColorButton("Live Data", liveState.uniqueCol);
    ImGui::SameLine();
    ImGui::Checkbox("Live Data##liveData", &liveVisible);

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

    liveState.active = liveActive;
    liveState.visible = liveVisible;
}

const StateData& StateManager::getLive() const noexcept
{
    return liveState;
}

const std::list<StateData>& StateManager::getSaved() const noexcept
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
    liveActive = false;
    for (auto& state : saved) {
        state.active = false;
    }
}
