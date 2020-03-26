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

#include <sstream>
#include <stack>

template <class T>
std::string toStringPrecision(T value, long precision)
{
    std::stringstream ss;
    ss.precision(precision);
    ss << value;
    return ss.str();
}

static std::stack<PlotConfig> gConfigStack;

const ImColor gridColor = ImColor(0.5F, 0.5F, 0.6F, 1.0F);

// log convert and log convert back both operate between 0 and 1
double logConvert(double value, double min, double max, bool isLog)
{
    if (isLog) {
        value = std::log(value / min) / std::log(max / min);
    }

    return std::max(0.0, std::min(1.0, value));
}

double logConvertBack(double value, double min, double max, bool isLog)
{
    if (isLog) {
        value = std::pow(max, value) * std::pow(min, 1.0 - value);
    }

    return std::max(0.0, std::min(1.0, value));
}

size_t pixelToIndex(double pixel, float pixels, bool isLog, size_t arraySize)
{
    double linear = pixel / static_cast<double>(pixels);
    double min = 1.0 / pixels;
    double max = 1.0;

    double converted = logConvertBack(linear, min, max, isLog);
    double t = converted * static_cast<double>(arraySize);
    return static_cast<size_t>(std::round(t));
}

double valueToPixel(double value, double min, double max, bool isLog, float pixels)
{
    double linear = (value - min) / (max - min);
    return logConvert(linear, min, max, isLog) * static_cast<double>(pixels);
}

double pixelToValue(double pixel, double min, double max, bool isLog, float pixels)
{
    double linear = logConvertBack(pixel / static_cast<double>(pixels), min, max, isLog);
    return min + linear * (max - min);
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
    /*
    if (config.yGridInterval != 0.0) {
        for (size_t i = 0; config.min + static_cast<double>(i) * config.yGridInterval < config.max; ++i) {
            double yVal = std::round((config.min + static_cast<double>(i) * config.yGridInterval)/config.yGridInterval) * config.yGridInterval;
            auto y = static_cast<float>(makeY(config, frame_bb.GetHeight(), yVal));
            ImVec2 p0(0.0, y);
            p0 += frame_bb.Min;
            ImVec2 p1(frame_bb.GetWidth(), y);
            p1 += frame_bb.Min;
            ImGui::GetWindowDrawList()->AddLine(p0, p1, gridColor);
            auto labelStr = toStringPrecision(yVal, 2);
            ImGui::GetWindowDrawList()->AddText(p0, gridColor, labelStr.c_str());
        }
    }

    // x grid
    if (config.xGridInterval != 0.0) {
        for (size_t i = 0; config.valueMin + static_cast<double>(i) * config.xGridInterval < config.valueMax; ++i) {
            double xVal = std::round((config.valueMin + static_cast<double>(i) * config.xGridInterval)/config.xGridInterval) * config.xGridInterval;
            auto x = static_cast<float>(makeX(config, frame_bb.GetWidth(), xVal));
            ImVec2 p0(x, 0.0);
            p0 += frame_bb.Min;
            ImVec2 p1(x, frame_bb.GetHeight());
            p1 += frame_bb.Min;
            ImGui::GetWindowDrawList()->AddLine(p0, p1, gridColor);
            auto labelStr = toStringPrecision(xVal, 2);
            ImGui::GetWindowDrawList()->AddText(p1, gridColor, labelStr.c_str());
        }
    }
     */
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

    double v0 = callback(0);

    double lastx = 0;
    double lastY = valueToPixel(v0, config.yMin, config.yMax, config.yLogscale, inner_bb.GetHeight());

    for (int x = 1; x < static_cast<int>(inner_bb.GetWidth()); x++) {
        auto newX = static_cast<double>(x);
        size_t t = pixelToIndex(newX, inner_bb.GetWidth(), config.xLogscale, config.count);
        double v = callback(t);
        auto newY = valueToPixel(v, config.yMin, config.yMax, config.yLogscale, inner_bb.GetHeight());

        ImVec2 pos1 = inner_bb.Min + ImVec2(static_cast<float>(newX), inner_bb.GetHeight() - static_cast<float>(newY));
        ImVec2 pos0 = inner_bb.Min + ImVec2(static_cast<float>(lastx), inner_bb.GetHeight() - static_cast<float>(lastY));
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
        auto x = static_cast<double>(g.IO.MousePos.x - inner_bb.Min.x);
        size_t t = pixelToIndex(x, inner_bb.GetWidth(), config.xLogscale, config.count);
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
