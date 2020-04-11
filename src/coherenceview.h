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

#ifndef laa_coherenceview_h
#define laa_coherenceview_h

#include "statemanager.h"

class CoherenceView {
public:
    void update(StateManager& stateManager, std::string idHint);

private:
    float min = 30.0F;
    float max = 20000.0F;
    bool smoothing = true;
};

#endif //laa_coherenceview_h