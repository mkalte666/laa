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

// clang-format off
#include <complex.h>
#include <fftw3.h>
// clang-format on

template <class T>
class FFTWAllocator : public std::allocator<T> {
public:
    template <typename U>
    struct rebind {
        typedef FFTWAllocator other;
    };
    T* allocate(size_t n)
    {
        return reinterpret_cast<T*>(fftw_malloc(sizeof(T) * n));
    }
    void deallocate(T* data, size_t)
    {
        fftw_free(data);
    }
};

using Real = double;
using RealVec = std::vector<Real, FFTWAllocator<Real>>;
using Complex = double _Complex;
using ComplexVec = std::vector<Complex, FFTWAllocator<Complex>>;

inline double real(const Complex& c)
{
    return creal(c);
}

inline double imag(const Complex& c)
{
    return cimag(c);
}

#define EPSILON 0.000001
inline double phase(const Complex& c)
{
    return carg(c);
    /*
    double preAtan = 0.0;
    if (std::abs(real(c)) < 0.01) {
        return 0.0;
    }
    if (std::abs(real(c)) < EPSILON) {
        preAtan = imag(c) > 0.0 ? M_PI / 2.0 : -M_PI / 2.0;
    } else {
        preAtan = imag(c) / real(c);
    }

    auto p = std::atan(preAtan);

    if (real(c) < 0) {
        if (imag(c) < 0) {
            p -= M_PI;
        } else {
            p += M_PI;
        }
    }

    return p;
     */
}

inline double mag(const Complex& c)
{
    return cabs(c);
}
#endif //LAA_FFT_H
