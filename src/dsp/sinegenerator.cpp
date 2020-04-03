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

#include "sinegenerator.h"
#include <cmath>

double SineGenerator::nextSample()
{
    auto res = std::sin(M_PI * 2.0 * currentTime * freq);
    currentTime += 1.0 / sampleRate;
    return res;
}

double SineGenerator::getFrequency() const
{
    return freq;
}

void SineGenerator::setFrequency(double f)
{
    if (f < 0.00000001) {
        return;
    }
    currentTime = (currentTime * freq) / f;
    freq = f;
}

double SineGenerator::getSampleRate() const
{
    return sampleRate;
}

void SineGenerator::setSampleRate(double rate)
{
    currentTime = 0.0;
    sampleRate = rate;
}
