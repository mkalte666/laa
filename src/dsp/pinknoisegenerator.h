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

#ifndef laa_pinknoisegenerator_h
#define laa_pinknoisegenerator_h

#include "whitenoisegenerator.h"
#include <valarray>

// stolen hard from http://www.firstpr.com.au/dsp/pink-noise/#Filtering
class PinkNoiseGenerator {
public:
    double nextSample() noexcept;

private:
    double b0 = 0.0;
    double b1 = 0.0;
    double b2 = 0.0;
    double b3 = 0.0;
    double b4 = 0.0;
    double b5 = 0.0;
    double b6 = 0.0;
};

#endif //laa_pinknoisegenerator_h
