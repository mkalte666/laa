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

#ifndef LAA_SINEGENERATOR_H
#define LAA_SINEGENERATOR_H

class SineGenerator {
public:
    double nextSample();
    double getFrequency() const;
    void setFrequency(double f);

private:
    double freq = 1000.0;
    double sampleRate = 48000.0;
    double currentTime = 0.0;

public:
    double getSampleRate() const;
    void setSampleRate(double sampleRate);
};

#endif //LAA_SINEGENERATOR_H
