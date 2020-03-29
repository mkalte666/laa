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

#ifndef laa_viewmanager_h
#define laa_viewmanager_h

#include "audiohandler.h"
#include "freqview.h"
#include "irview.h"
#include "magview.h"
#include "phaseview.h"
#include "shared.h"
#include "signalview.h"
#include "statemanager.h"

enum class View {
    Signal,
    Magnitude,
    Phase,
    ImpulseResponse,
    FrequencyResponse
};

class ViewManager {
public:
    void update(ImVec2 windowSize) noexcept;

private:
    void drawSelectorAndContent(ImVec2 windowSize, float offset, View& view) noexcept;

    AudioHandler audioHandler = {};
    StateManager stateManager = {};
    SignalView signalView = {};
    MagView fftView = {};
    PhaseView phaseView = {};
    IrView irView = {};
    FreqView freqView = {};
    View upper = View::Magnitude;
    View lower = View::Signal;
};

#endif //laa_viewmanager_h
