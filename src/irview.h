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

#ifndef laa_irview_h
#define laa_irview_h

#include "audio/audiohandler.h"
#include "dsp/fft.h"
#include "statemanager.h"

struct IrMarker {
    PlotClickInfo clickInfo = {};
    ImColor color = {};
    bool isRef = false;
};

class IrView {
public:
    void update(StateManager& stateManager, std::string idHint);

private:
    void addMarker(const StateData& state, const PlotClickInfo& info) noexcept;
    double range = 1.0;
    double yRange = 0.51;
    bool smoothing = false;
    bool showAbsValues = false;

    std::list<IrMarker> markers = {};
    double refValue = 0.0;
    void clearRef() noexcept;
    void findPeak(StateManager& stateManager) noexcept;
};
#endif //laa_irview_h
