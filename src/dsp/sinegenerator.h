//
// Created by mkalte on 19/03/2020.
//

#ifndef LAA_SINEGENERATOR_H
#define LAA_SINEGENERATOR_H

class SineGenerator {
public:
    double nextSample();
    double getFrequency() const;
    void setFrequency(double f);

private:
    double freq = 1000.0;
    double sampleRate = 48000.0;
    double currentTime = 0.0;

public:
    double getSampleRate() const;
    void setSampleRate(double sampleRate);
};

#endif //LAA_SINEGENERATOR_H
