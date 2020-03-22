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

#include "signalview.h"
#include "plot.h"

void SignalView::update(ImVec2 windowSize, StateManager& stateManager) noexcept
{
    ImGui::SetNextWindowPos(ImVec2(windowSize.x / 6.0F, 0.0F));
    auto size = ImVec2(windowSize.x * 5.0F / 6.0F, windowSize.y);
    ImGui::SetNextWindowSize(size);

    const auto& liveState = stateManager.getLive();
    const auto& data = liveState.input;

    PlotConfig plotConfig;
    plotConfig.size = size;
    plotConfig.count = data.size();
    plotConfig.min = -1.0;
    plotConfig.max = 1.0;
    plotConfig.label = "Signal";

    ImGui::Begin("Signal");
    BeginPlot(plotConfig);
    Plot([&data](size_t idx) {
        return data[idx];
    });
    EndPlot();
    ImGui::End();
}