//
// Created by mkalte on 03/04/2020.
//

#ifndef LAA_SMOOTHING_H
#define LAA_SMOOTHING_H

#include <cmath>
#include <vector>

template <class T, class Talloc = std::allocator<double>>
void smooth(std::vector<T, Talloc>& out, const std::vector<T, Talloc>& in, size_t maxLen = 0)
{
    if (out.size() != in.size()) {
        out.resize(in.size(), 0.0F);
    }
    if (maxLen == 0) {
        maxLen = out.size();
    }
    for (size_t writeIndex = 0; writeIndex < maxLen; ++writeIndex) {
        size_t readStart = writeIndex > 10 ? writeIndex - 10 : 0;
        size_t readEnd = writeIndex + 10;
        for (size_t readIndex = readStart; readIndex < readEnd && readIndex < maxLen; ++readIndex) {
            double absDist = std::abs(static_cast<double>(readEnd - readStart));
            out[writeIndex] += in[readIndex] / absDist;
        }
    }
}

#endif //LAA_SMOOTHING_H
