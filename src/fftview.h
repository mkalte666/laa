//
// Created by mkalte on 20/03/2020.
//

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
