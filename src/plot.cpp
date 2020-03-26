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

struct InternalPlotConfig {
    ImVec2 labelSize = {};
    ImRect frameBb = {};
    ImRect innerBb = {};
    ImRect totalBb = {};
    bool skipped = false;
    ImGuiWindow* window = nullptr;
};

static std::stack<PlotConfig> gConfigStack;
static std::stack<InternalPlotConfig> gInternalConfigStack;

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

std::vector<double> calcGridValues(double min, double max, double interval, double hint, bool isLog)
{
    std::vector<double> result;

    if (isLog) {
        return result;
    }

    auto valUp = hint;
    auto valDown = hint;
    for (long i = 0; valUp < max || valDown > min; ++i) {
        if (valUp < max) {
            result.push_back(valUp);
        }

        if (i != 0 && valDown > min) {
            result.push_back(valDown);
        }
        valUp = hint + static_cast<double>(i) * interval;
        valDown = hint - static_cast<double>(i) * interval;
    }

    return result;
}

void BeginPlot(const PlotConfig& config) noexcept
{
    gConfigStack.push(config);
    InternalPlotConfig internalConfig;
    internalConfig.window = ImGui::GetCurrentWindow();
    internalConfig.skipped = internalConfig.window->SkipItems;

    if (internalConfig.skipped) {
        return;
    }

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    internalConfig.labelSize = ImGui::CalcTextSize(config.label.c_str(), NULL, true);
    internalConfig.frameBb = ImRect(internalConfig.window->DC.CursorPos, internalConfig.window->DC.CursorPos + config.size);
    internalConfig.innerBb = ImRect(internalConfig.frameBb.Min + style.FramePadding, internalConfig.frameBb.Max - style.FramePadding);
    internalConfig.totalBb = ImRect(internalConfig.frameBb.Min, internalConfig.frameBb.Max + ImVec2(internalConfig.labelSize.x > 0.0f ? style.ItemInnerSpacing.x + internalConfig.labelSize.x : 0.0f, 0));
    ImGui::RenderFrame(internalConfig.frameBb.Min, internalConfig.frameBb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    auto xGridVals = calcGridValues(config.xMin, config.xMax, config.xGridInterval, config.xGridHint, config.xLogscale);
    for (auto&& xGridVal : xGridVals) {
        auto pixelX = valueToPixel(xGridVal, config.xMin, config.xMax, config.xLogscale, internalConfig.innerBb.GetWidth());
        ImVec2 p0 = internalConfig.innerBb.Min + ImVec2(static_cast<float>(pixelX), 0.0);
        ImVec2 p1 = internalConfig.innerBb.Min + ImVec2(static_cast<float>(pixelX), internalConfig.innerBb.GetHeight());
        internalConfig.window->DrawList->AddLine(p0, p1, gridColor);
        internalConfig.window->DrawList->AddText(p1, gridColor, toStringPrecision(xGridVal, 2).c_str());
    }

    auto yGridVals = calcGridValues(config.yMin, config.yMax, config.yGridInterval, config.yGridHint, config.yLogscale);
    for (auto&& yGridVal : yGridVals) {
        auto pixelY = valueToPixel(yGridVal, config.yMin, config.yMax, config.yLogscale, internalConfig.innerBb.GetHeight());
        ImVec2 p0 = internalConfig.innerBb.Min + ImVec2(0.0, static_cast<float>(pixelY));
        ImVec2 p1 = internalConfig.innerBb.Min + ImVec2(internalConfig.innerBb.GetWidth(), static_cast<float>(pixelY));
        internalConfig.window->DrawList->AddLine(p0, p1, gridColor);
        internalConfig.window->DrawList->AddText(p0, gridColor, toStringPrecision(yGridVal, 2).c_str());
    }
    gInternalConfigStack.push(internalConfig);
}

void Plot(PlotCallback callback, ImColor const* col) noexcept
{
    SDL2WRAP_ASSERT(!gConfigStack.empty());
    SDL2WRAP_ASSERT(gConfigStack.size() == gInternalConfigStack.size());
    auto config = gConfigStack.top();
    auto internalConfig = gInternalConfigStack.top();

    if (!col) {
        col = &config.color;
    }

    const ImGuiID id = internalConfig.window->GetID(config.label.c_str());
    const bool hovered = ImGui::ItemHoverable(internalConfig.frameBb, id);

    if (config.count < 2u) {
        return;
    }

    double v0 = callback(0);

    double lastx = 0;
    double lastY = valueToPixel(v0, config.yMin, config.yMax, config.yLogscale, internalConfig.innerBb.GetHeight());

    for (int x = 1; x < static_cast<int>(internalConfig.innerBb.GetWidth()); x++) {
        auto newX = static_cast<double>(x);
        size_t t = pixelToIndex(newX, internalConfig.innerBb.GetWidth(), config.xLogscale, config.count);
        double v = callback(t);
        auto newY = valueToPixel(v, config.yMin, config.yMax, config.yLogscale, internalConfig.innerBb.GetHeight());

        ImVec2 pos1 = internalConfig.innerBb.Min + ImVec2(static_cast<float>(newX), internalConfig.innerBb.GetHeight() - static_cast<float>(newY));
        ImVec2 pos0 = internalConfig.innerBb.Min + ImVec2(static_cast<float>(lastx), internalConfig.innerBb.GetHeight() - static_cast<float>(lastY));
        internalConfig.window->DrawList->AddLine(pos0, pos1, *col);
        lastx = newX;
        lastY = newY;
    }

    if (hovered && internalConfig.innerBb.Contains(GImGui->IO.MousePos)) {
        ImVec2 pos0 = internalConfig.innerBb.Min;
        pos0.x = GImGui->IO.MousePos.x;
        ImVec2 pos1 = internalConfig.innerBb.Max;
        pos1.x = GImGui->IO.MousePos.x;
        internalConfig.window->DrawList->AddLine(pos0, pos1, 0xFFFFFFFFu);
        auto x = static_cast<double>(GImGui->IO.MousePos.x - internalConfig.innerBb.Min.x);
        size_t t = pixelToIndex(x, internalConfig.innerBb.GetWidth(), config.xLogscale, config.count);
        double v = callback(t);
        ImGui::SetTooltip("%f", v);
    }
}

void EndPlot() noexcept
{
    SDL2WRAP_ASSERT(!gConfigStack.empty());
    SDL2WRAP_ASSERT(gConfigStack.size() == gInternalConfigStack.size());
    auto config = gConfigStack.top();
    auto internalConfig = gInternalConfigStack.top();

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    ImGui::ItemSize(internalConfig.totalBb, style.FramePadding.y);
    if (!ImGui::ItemAdd(internalConfig.totalBb, 0, &internalConfig.frameBb)) {
        return;
    }

    gConfigStack.pop();
    gInternalConfigStack.pop();
}
