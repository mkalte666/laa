//
// Created by mkalte on 19/03/2020.
//

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
    currentTime = 0.0;
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
