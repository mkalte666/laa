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

float viewAreaWidth(ImVec2 windowSize) noexcept
{
    return windowSize.x - sidebarWidth(windowSize);
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

    drawSelectorAndContent(windowSize, 0.0F);
    drawSelectorAndContent(windowSize, halfHeight(windowSize));
}

void ViewManager::drawSelectorAndContent(ImVec2 windowSize, float offset) noexcept
{
    std::string idHint = offset > 0.0F ? "one" : "two";

    // the actually selected window
    ImGui::SetNextWindowPos(ImVec2(sidebarWidth(windowSize), offset));
    ImGui::SetNextWindowSize(ImVec2(viewAreaWidth(windowSize), halfHeight(windowSize)));
    ImGui::Begin((idHint + "plot view").c_str(), nullptr, ImGuiWindowFlags_NoDecoration);
    ImGui::PushID(idHint.c_str());
    ImGui::BeginTabBar("View Select bar");

    if (ImGui::BeginTabItem("Signal")) {
        signalView.update(stateManager, idHint);
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Magnitude")) {
        fftView.update(stateManager, idHint);
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Phase")) {
        phaseView.update(stateManager, idHint);
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("FR")) {
        freqView.update(stateManager, idHint);
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("IR")) {
        irView.update(stateManager, idHint);
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Coherence")) {
        coherenceView.update(stateManager, idHint);
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
    ImGui::PopID();
    ImGui::End();
}
