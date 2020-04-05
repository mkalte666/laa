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

#include "viewmanager.h"

float sidebarWidth(ImVec2 windowSize) noexcept
{
    float f = windowSize.x / 6.0F;

    return std::min(200.0F, std::max(150.0F, f));
}

float viewSelectorWidth(ImVec2 windowSize) noexcept
{
    return sidebarWidth(windowSize) / 1.5F;
}

float viewAreaWidth(ImVec2 windowSize) noexcept
{
    return windowSize.x - viewSelectorWidth(windowSize) - sidebarWidth(windowSize);
}

float halfHeight(ImVec2 windowSize) noexcept
{
    return windowSize.y / 2.0F;
}

void ViewManager::update(ImVec2 windowSize) noexcept
{
    ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F));
    ImGui::SetNextWindowSize(ImVec2(sidebarWidth(windowSize), halfHeight(windowSize)));
    audioHandler.update();

    ImGui::SetNextWindowPos(ImVec2(0.0F, halfHeight(windowSize)));
    ImGui::SetNextWindowSize(ImVec2(sidebarWidth(windowSize), halfHeight(windowSize)));
    stateManager.update(audioHandler);

    drawSelectorAndContent(windowSize, 0.0F, upper);
    drawSelectorAndContent(windowSize, halfHeight(windowSize), lower);
}

void ViewManager::drawSelectorAndContent(ImVec2 windowSize, float offset, View& view) noexcept
{
    std::string idHint = offset > 0.0F ? "one" : "two";
    ImGui::PushID(idHint.c_str());

    // window selector
    ImGui::SetNextWindowPos(ImVec2(sidebarWidth(windowSize), offset));
    ImGui::SetNextWindowSize(ImVec2(viewSelectorWidth(windowSize), halfHeight(windowSize)));
    ImGui::Begin(idHint.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);
    ImGui::PushItemWidth(-1);
    ImGui::Text("Signal");
    if (ImGui::Button("Signal")) {
        view = View::Signal;
    }
    ImGui::Text("DFT");
    if (ImGui::Button("Magnitude")) {
        view = View::Magnitude;
    }
    if (ImGui::Button("Phase")) {
        view = View::Phase;
    }
    if (ImGui::Button("IR")) {
        view = View::ImpulseResponse;
    }
    if (ImGui::Button("FR")) {
        view = View::FrequencyResponse;
    }
    ImGui::PopItemWidth();
    ImGui::End();

    // the actually selected window
    ImGui::SetNextWindowPos(ImVec2(sidebarWidth(windowSize) + viewSelectorWidth(windowSize), offset));
    ImGui::SetNextWindowSize(ImVec2(viewAreaWidth(windowSize), halfHeight(windowSize)));
    switch (view) {

    case View::Signal:
        signalView.update(stateManager, idHint);
        break;
    case View::Magnitude:
        fftView.update(stateManager, idHint);
        break;
    case View::Phase:
        phaseView.update(stateManager, idHint);
        break;
    case View::ImpulseResponse:
        irView.update(stateManager, idHint);
        break;
    case View::FrequencyResponse:
        freqView.update(stateManager, idHint);
        break;
    }

    ImGui::PopID();
}
