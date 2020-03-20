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

#include "pinknoisegenerator.h"

double PinkNoiseGenerator::nextSample() noexcept
{
    const double gainFactor = 0.1;
    double white = WhiteNoiseGenerator::nextSample();
    b0 = 0.99886 * b0 + white * 0.0555179 * gainFactor;
    b1 = 0.99332 * b1 + white * 0.0750759 * gainFactor;
    b2 = 0.96900 * b2 + white * 0.1538520 * gainFactor;
    b3 = 0.86650 * b3 + white * 0.3104856 * gainFactor;
    b4 = 0.55000 * b4 + white * 0.5329522 * gainFactor;
    b5 = -0.7616 * b5 - white * 0.0168980 * gainFactor;
    double pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362 * gainFactor;
    b6 = white * 0.115926 * gainFactor;

    return pink;
}
