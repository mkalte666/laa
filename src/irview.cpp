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

#include "irview.h"
#include "dsp/peak.h"

void IrView::update(StateManager& stateManager, std::string idHint)
{
    ImGui::BeginChild((idHint + "Mag").c_str());
    auto size = ImGui::GetWindowContentRegionMax();

    // begin with the plot
    ImGui::Columns(2);
    ImGui::SetColumnWidth(-1, size.x * 0.75F);

    const auto& liveState = stateManager.getLive();
    const auto& data = choose(smoothing, liveState.smoothedImpulseResponse, liveState.impulseResponse);
    // make sure range doesn't clip
    range = std::clamp(range, 0.0, liveState.fftDuration);

    PlotConfig plotConfig;
    plotConfig.label = "IR View";
    plotConfig.size = ImVec2(size.x * 0.7F, size.y * 0.8F);
    plotConfig.yAxisConfig.min = -0.51;
    plotConfig.yAxisConfig.max = 0.51;
    plotConfig.yAxisConfig.gridInterval = 0.1;

    plotConfig.xAxisConfig.min = -0.05F;
    plotConfig.xAxisConfig.max = range;
    plotConfig.xAxisConfig.gridInterval = 0.05;
    if (range < 0.1) {
        plotConfig.xAxisConfig.gridInterval = 0.01;
        plotConfig.xAxisConfig.min = -0.01F;
    }
    BeginPlot(plotConfig);
    if (liveState.visible) {
        PlotSourceConfig sourceConfig;
        sourceConfig.xMin = 0.0;
        sourceConfig.xMax = liveState.fftDuration;
        sourceConfig.color = liveState.uniqueCol;
        sourceConfig.active = liveState.active;
        sourceConfig.count = liveState.fftLen;
        sourceConfig.antiAliasingBehaviour = AntiAliasingBehaviour::AbsMax;
        auto clicked = Plot(
            sourceConfig,
            [&data](size_t idx) {
                return data[idx];
            });
        if (clicked) {
            addMarker(liveState, clicked);
        }
    }

    for (auto& state : stateManager.getSaved()) {
        if (!state.visible) {
            continue;
        }
        const auto& stateData = choose(smoothing, state.smoothedImpulseResponse, state.impulseResponse);
        PlotSourceConfig sourceConfig;
        sourceConfig.count = state.fftLen;
        sourceConfig.xMin = 0.0;
        sourceConfig.xMax = state.fftDuration;
        sourceConfig.color = state.uniqueCol;
        sourceConfig.active = state.active;
        sourceConfig.antiAliasingBehaviour = AntiAliasingBehaviour::AbsMax;
        auto clicked = Plot(
            sourceConfig, [&stateData](size_t idx) {
                return stateData[idx];
            });
        if (clicked) {
            addMarker(state, clicked);
        }
    }

    // draw the markers
    PlotMarkerConfig markerConfig = {};
    markerConfig.drawYLine = true;
    markerConfig.enableCustomLabel = true;

    for (const auto& marker : markers) {
        markerConfig.color = marker.color;
        markerConfig.customLabel = std::to_string(marker.clickInfo.x - refValue);
        if (marker.isRef) {
            markerConfig.customLabel = "Reference";
        }
        PlotMarker(markerConfig, marker.clickInfo);
    }

    EndPlot();

    float fRange = static_cast<float>(range);
    ImGui::SliderFloat("Range", &fRange, 0.0F, static_cast<float>(liveState.fftDuration), "%.3f", 5.0F);
    range = static_cast<double>(fRange);
    ImGui::Checkbox("Enable Smoothing", &smoothing);

    // now, marker selection
    ImGui::NextColumn();
    ImGui::SetColumnWidth(-1, size.x * 0.25F);
    ImGui::BeginChild("marker list");
    ImGui::PushItemWidth(-1.0F);
    ImGui::Text("Markers");
    if (ImGui::Button("Find Peak")) {
        findPeak(stateManager);
    }
    if (ImGui::Button("Reset Reference")) {
        clearRef();
    }

    for (auto markerIter = markers.begin(); markerIter != markers.end(); ++markerIter) {
        ImGui::PushID(&(*markerIter));
        if (ImGui::RadioButton("##markerRefLabel", markerIter->isRef)) {
            clearRef();
            markerIter->isRef = true;
            refValue = markerIter->clickInfo.x;
        }
        ImGui::SameLine();
        ImGui::TextColored(markerIter->color, "%+.4f", markerIter->clickInfo.x - refValue);
        ImGui::SameLine();

        // needs to stay last, or markerIter might be end() when used!
        if (ImGui::Button("x##deleteMarker")) {
            if (markerIter->isRef) {
                clearRef();
            }
            markerIter = markers.erase(markerIter);
            ImGui::PopID(); // FIXME: this flickers, but less than the original solution
            break;
        }
        ImGui::PopID();
    }
    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::Columns(1);
    ImGui::EndChild();
}

void IrView::addMarker(const StateData& state, const PlotClickInfo& info) noexcept
{
    IrMarker marker;
    marker.clickInfo = info;
    marker.color = state.uniqueCol;
    markers.push_back(marker);
}
void IrView::clearRef() noexcept
{
    refValue = 0.0;
    for (auto& marker : markers) {
        marker.isRef = 0;
    }
}

IrMarker makeMarkerFromPeak(const StateData& stateData)
{
    size_t index = findMax(stateData.impulseResponse);
    double yVal = stateData.impulseResponse[index];
    double xVal = stateData.fftDuration * static_cast<double>(index) / static_cast<double>(stateData.fftLen);

    IrMarker result = {};
    result.clickInfo.clicked = true;
    result.clickInfo.x = xVal;
    result.clickInfo.y = yVal;
    result.color = stateData.uniqueCol;

    return result;
}

void IrView::findPeak(StateManager& stateManager) noexcept
{
    IrMarker marker = {};
    bool anyActive = false;
    const auto& liveState = stateManager.getLive();
    if (liveState.active) {
        anyActive = true;
        marker = makeMarkerFromPeak(liveState);
    } else {
        for (const auto& state : stateManager.getSaved()) {
            if (state.active) {
                marker = makeMarkerFromPeak(state);
                anyActive = true;
                break;
            }
        }
    }

    if (anyActive) {
        markers.push_back(marker);
    }
}
