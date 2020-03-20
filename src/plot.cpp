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

#include "plot.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

double makeY(double min, double max, double height, double value)
{
    return (((max - min) - (value - min)) / (max - min)) * height;
}

void Plot(const PlotConfig& config) noexcept
{
    auto* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return;
    }

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    //const ImGuiID id = window->GetID(config.xLabel.c_str());
    const ImVec2 label_size = ImGui::CalcTextSize(config.xLabel.c_str(), NULL, true);
    ImVec2 frame_size = config.size;
    if (frame_size.x == 0.0f)
        frame_size.x = ImGui::CalcItemWidth();
    if (frame_size.y == 0.0f)
        frame_size.y = label_size.y + (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, 0, &frame_bb)) {
        return;
    }
    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    if (config.count < 2u) {
        return;
    }

    double step = static_cast<double>(config.count) / static_cast<double>(inner_bb.GetWidth());
    double v0 = config.callback(0);

    double lastx = 0;
    double lastY = static_cast<int>(makeY(config.min, config.max, static_cast<double>(frame_bb.GetHeight()), v0));

    for (int x = 1; x < static_cast<int>(inner_bb.GetWidth()); x++) {
        size_t t = static_cast<size_t>(std::floor(static_cast<double>(x) * step));
        double v = config.callback(t);

        double newX = static_cast<double>(x);
        auto newY = makeY(config.min, config.max, static_cast<double>(frame_bb.GetHeight()), v);

        ImVec2 pos1 = inner_bb.Min + ImVec2(static_cast<float>(newX), static_cast<float>(newY));
        ImVec2 pos0 = inner_bb.Min + ImVec2(static_cast<float>(lastx), static_cast<float>(lastY));
        window->DrawList->AddLine(pos0, pos1, 0xFFFFFFFFu);
        lastx = newX;
        lastY = newY;
    }
}
