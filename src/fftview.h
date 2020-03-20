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

#ifndef LAA_FFTVIEW_H
#define LAA_FFTVIEW_H

#include "audiohandler.h"
#include "dsp/fft.h"

class FftView {
public:
    void update(AudioHandler& audioHandler);

private:
    size_t lastFrame = 0;
    std::vector<double> reference;
    std::vector<double> input;
    ComplexVec refFft;
    ComplexVec refFftAvg;
    ComplexVec inFft;
    ComplexVec inFftAvg;
    ComplexVec H;
    std::vector<double> h;
};

#endif //LAA_FFTVIEW_H
