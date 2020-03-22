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
#include "shared.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include <stack>
static std::stack<PlotConfig> gConfigStack;

double makeY(const PlotConfig& config, double height, double value)
{
    return (((config.max - config.min) - (value - config.min)) / (config.max - config.min)) * height;
}

void BeginPlot(const PlotConfig& config) noexcept
{
    gConfigStack.push(config);

    auto* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return;
    }

    ImGuiContext& g = *GImGui;
    ImVec2 frame_size = config.size;
    const ImGuiStyle& style = g.Style;
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);

    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
    // y grid
    if (config.yGridInterval != 0.0) {
        for (size_t i = 0; static_cast<double>(i) * config.yGridInterval < config.max || static_cast<double>(i) * config.yGridInterval * -1.0 > config.min; ++i) {
            double yUp = static_cast<double>(i) * config.yGridInterval;
            double yDown = static_cast<double>(i) * config.yGridInterval * -1.0;
            if (yUp < config.max) {
                auto y = static_cast<float>(makeY(config, frame_bb.GetHeight(), yUp));
                ImVec2 p0(0.0, y);
                p0 += frame_bb.Min;
                ImVec2 p1(frame_bb.GetWidth(), y);
                p1 += frame_bb.Min;
                ImGui::GetWindowDrawList()->AddLine(p0, p1, 0xFFFFFFFF);
            }

            if (yDown > config.min) {
                auto y = static_cast<float>(makeY(config, frame_bb.GetHeight(), yDown));
                ImVec2 p0(0.0, y);
                p0 += frame_bb.Min;
                ImVec2 p1(frame_bb.GetWidth(), y);
                p1 += frame_bb.Min;
                ImGui::GetWindowDrawList()->AddLine(p0, p1, 0xFFFFFFFF);
            }
        }
    }
}

void Plot(PlotCallback callback, ImColor const* col) noexcept
{
    auto config = gConfigStack.top();
    if (!col) {
        col = &config.color;
    }
    auto* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return;
    }

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(config.label.c_str());
    const ImVec2 label_size = ImGui::CalcTextSize(config.label.c_str(), NULL, true);
    ImVec2 frame_size = config.size;
    if (frame_size.x == 0.0f)
        frame_size.x = ImGui::CalcItemWidth();
    if (frame_size.y == 0.0f)
        frame_size.y = label_size.y + (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);

    const bool hovered = ImGui::ItemHoverable(frame_bb, id);

    if (config.count < 2u) {
        return;
    }

    double step = static_cast<double>(config.count) / static_cast<double>(inner_bb.GetWidth());
    double v0 = callback(0);

    double lastx = 0;
    double lastY = static_cast<int>(makeY(config, static_cast<double>(frame_bb.GetHeight()), v0));

    for (int x = 1; x < static_cast<int>(inner_bb.GetWidth()); x++) {
        size_t t = static_cast<size_t>(std::floor(static_cast<double>(x) * step));
        double v = callback(t);

        double newX = static_cast<double>(x);
        auto newY = makeY(config, static_cast<double>(frame_bb.GetHeight()), v);

        ImVec2 pos1 = inner_bb.Min + ImVec2(static_cast<float>(newX), static_cast<float>(newY));
        ImVec2 pos0 = inner_bb.Min + ImVec2(static_cast<float>(lastx), static_cast<float>(lastY));
        window->DrawList->AddLine(pos0, pos1, *col);
        lastx = newX;
        lastY = newY;
    }

    if (hovered && inner_bb.Contains(g.IO.MousePos)) {
        ImVec2 pos0 = inner_bb.Min;
        pos0.x = g.IO.MousePos.x;
        ImVec2 pos1 = inner_bb.Max;
        pos1.x = g.IO.MousePos.x;
        window->DrawList->AddLine(pos0, pos1, 0xFFFFFFFFu);
        size_t t = static_cast<size_t>(std::floor(static_cast<double>(g.IO.MousePos.x - inner_bb.Min.x) * step));
        double v = callback(t);
        ImGui::SetTooltip("%f", v);
    }
}

void EndPlot() noexcept
{
    SDL2WRAP_ASSERT(!gConfigStack.empty());
    auto config = gConfigStack.top();
    auto* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return;
    }

    ImGuiContext& g = *GImGui;
    ImVec2 frame_size = config.size;
    const ImGuiStyle& style = g.Style;
    const ImVec2 label_size = ImGui::CalcTextSize(config.label.c_str(), NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, 0, &frame_bb)) {
        return;
    }

    gConfigStack.pop();
}
