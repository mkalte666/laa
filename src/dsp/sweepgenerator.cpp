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

#include "sweepgenerator.h"
#include <cmath>

double SweepGenerator::nextSample() noexcept
{
    double res = std::sin(phi);
    phi += dt;
    currFreq += df;
    dt = 2.0 * M_PI * currFreq / sampleRate;

    if (currFreq >= fmax) {
        reset();
    }
    return res;
}

void SweepGenerator::setSampleRate(double rate) noexcept
{
    sampleRate = rate;
    reset();
}

double SweepGenerator::getLength() const noexcept
{
    return length;
}

void SweepGenerator::setLength(double newLength) noexcept
{
    length = newLength;
    reset();
}

void SweepGenerator::reset() noexcept
{
    fmin = 30.0;
    fmax = std::min(20000.0, sampleRate / 2.0);
    phi = 0.0;
    currFreq = fmin;
    dt = 2.0 * M_PI * currFreq;
    df = (fmax - fmin) / (sampleRate * length);
}
