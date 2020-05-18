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

#include "audioconfig.h"

std::vector<size_t> AudioConfig::getPossibleAnalysisSampleRates() noexcept
{
    std::vector<size_t> rates;
    for (size_t i = LAA_MIN_FFT_LENGTH; i < LAA_MAX_FFT_LENGTH; i <<= 1u) {
        rates.push_back(i);
    }

    return rates;
}

double AudioConfig::samplesToSeconds(size_t count) const noexcept
{
    return static_cast<double>(count) / static_cast<double>(sampleRate);
}

std::string AudioConfig::sampleCountToString(size_t count) const noexcept
{
    std::string result;
    double seconds = samplesToSeconds(count);
    result = std::to_string(count) + " (" + std::to_string(seconds) + "s / " + std::to_string(1.0 / seconds) + "Hz)";
    return result;
}
std::vector<unsigned int> AudioConfig::getLegalSampleRates() noexcept
{
    std::vector<unsigned int> result;
    auto capRates = captureDevice.sampleRates;
    auto playRates = playbackDevice.sampleRates;
    for (auto capRate : capRates) {
        for (auto playRate : playRates) {
            if (capRate == playRate && capRate < 100000 && capRate > 40000) {
                result.push_back(capRate);
                break;
            }
        }
    }
    return result;
}
