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
#include "../shared.h"
#include <cmath>

// https://ieeexplore.ieee.org/document/4813749

double SweepGenerator::nextSample() noexcept
{
    ++counter;
    double t = static_cast<double>(counter) / sampleRate;
    auto w = K / L * std::exp(t / L);
    // paper assumes n(0) = 1, but we dont want that as we want to stay in 0..1
    // nicer would be n(T) = 1. solve that =>
    auto n = std::sqrt(w / (2 * LAA_PI * fmax));
    auto res = n * std::sin(K * (std::exp(t / L) - 1.0));

    if (t > length) {
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
    fmax = sampleRate / 2;

    counter = 0;

    // K = T * w1 / (ln(w2/w1)
    K = length * (fmin * 2.0 * LAA_PI) / std::log(fmax / fmin);
    // L = T / ln(w1/w1)
    L = length / std::log(fmax / fmin);
}
