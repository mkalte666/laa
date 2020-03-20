//
// Created by mkalte on 20/03/2020.
//

#ifndef laa_hamming_h
#define laa_hamming_h

#include <cmath>
#include <vector>

template <class T>
inline void hamming(std::vector<T>& x)
{
    auto M = static_cast<double>(x.size());
    for (size_t i = 0; i < x.size(); i++) {
        auto di = static_cast<double>(i);
        x[i] *= 0.54 - 0.46 * std::cos(2.0 * M_PI * di / M);
    }
}

#endif //laa_hamming_h
