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

#ifndef laa_statemanager_h
#define laa_statemanager_h

#include "audiohandler.h"
#include <list>

ImColor randColor();

class StateManager {
public:
    StateManager() noexcept;
    ~StateManager() noexcept = default;
    void update(AudioHandler& audioHandler);

    [[nodiscard]] const StateData& getLive() const noexcept;

    [[nodiscard]] const std::list<StateData>& getSaved() const noexcept;

private:
    void deactivateAll();
    size_t lastFrame = 0;
    StateData liveState = {};

    std::list<StateData> saved;
};

#endif //laa_statemanager_h
