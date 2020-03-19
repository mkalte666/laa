//
// Created by mkalte on 19/03/2020.
//

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
