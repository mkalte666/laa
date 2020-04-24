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
#ifndef laa_sweepgenerator_h
#define laa_sweepgenerator_h

class SweepGenerator {
public:
    double nextSample() noexcept;
    void setSampleRate(double rate) noexcept;
    double getLength() const noexcept;
    void setLength(double newLength) noexcept;
    void reset() noexcept;

private:
    double fmin = 30.0;
    double fmax = 20000;
    double phi = 0.0;
    double currFreq = 0.0;
    double dt = 0.0;
    double df = 0.0;
    double sampleRate = 0.0;
    double length = 0.0;
};

#endif //laa_sweepgenerator_h
