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
#include <cstdlib>
#include <complex>
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
using Complex = std::complex<double>;
using ComplexVec = std::vector<Complex, FFTWAllocator<Complex>>;

inline double real(const Complex& c)
{
    return c.real();
}

inline double imag(const Complex& c)
{
    return c.imag();
}

inline double phase(const Complex& c)
{
    return std::arg(c);
}

inline double mag(const Complex& c)
{
    return std::abs(c);
}

inline double magSquared(const Complex& c)
{
    double real = c.real();
    double imag = c.imag();

    return real * real + imag * imag;
}

#endif //LAA_FFT_H
