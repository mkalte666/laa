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

#ifndef LAA_FFT_H
#define LAA_FFT_H

#include "../shared.h"

using Complex = std::complex<double>;
using ComplexVec = std::vector<Complex>;

template <class T>
inline void realToComplex(std::vector<std::complex<T>>& dst, const std::vector<T>& src)
{
    dst.resize(src.size());
    for (size_t i = 0; i < src.size(); i++) {
        dst[i].real(src[i]);
        dst[i].imag(0.0);
    }
}

template <class T>
inline void complexToReal(std::vector<T>& dst, const std::vector<std::complex<T>>& src)
{
    dst.resize(src.size());
    for (size_t i = 0; i < src.size(); i++) {
        dst[i] = src[i].real();
    }
}

inline void fftInplace(ComplexVec& x)
{
    const size_t N = x.size();
    if (N <= 1)
        return;

    // divide
    ComplexVec even;
    even.resize(N / 2);
    ComplexVec odd;
    odd.resize(N / 2);
    for (size_t i = 0; i < N / 2; i++) {
        even[i] = x[i * 2];
        odd[i] = x[i * 2 + 1];
    }

    // conquer
    fftInplace(even);
    fftInplace(odd);

    // combine
    for (size_t k = 0; k < N / 2; ++k) {
        Complex t = std::polar(1.0, -2.0 * M_PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;
    }
}

inline ComplexVec fft(const ComplexVec& x)
{
    auto result = x;
    fftInplace(result);
    return result;
}

inline ComplexVec fftReal(const std::vector<double>& x)
{
    ComplexVec result;
    realToComplex(result, x);
    fftInplace(result);
    return result;
}

inline void ifftInplace(ComplexVec& x)
{
    for (auto&& e : x) {
        e = std::conj(e);
    }

    fftInplace(x);

    auto dSize = static_cast<double>(x.size());
    for (auto&& e : x) {
        e = std::conj(e);
        e /= static_cast<double>(dSize);
    }
}

inline ComplexVec ifft(const ComplexVec& x)
{
    auto result = x;
    ifftInplace(result);
    return result;
}

inline std::vector<double> ifftReal(const ComplexVec& x)
{
    std::vector<double> result;
    complexToReal<double>(result, ifft(x));
    return result;
}

#endif //LAA_FFT_H
