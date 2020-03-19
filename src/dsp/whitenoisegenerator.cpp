//
// Created by mkalte on 19/03/2020.
//

#include "whitenoisegenerator.h"
#include <random>

double WhiteNoiseGenerator::nextSample() noexcept
{
    static std::random_device rd;
    static std::default_random_engine re(rd());
    static std::uniform_real_distribution<double> unif(-1.0, 1.0);
    return unif(re);
}
