#ifndef LPF_H
#define LPF_H

#include <cstdint>
#include <cmath>

class LPF {
    public:
        LPF(uint32_t fs, uint32_t f0, float q);
        ~LPF();
        float process(float x);
    private:
        uint32_t fs, f0;
        float q;
        float a0, a1, a2, b0, b1, b2;
        float x1, x2, y1, y2;
};

LPF::LPF(uint32_t samplingFrequency, uint32_t cutoffFrequency, float qualityFactor) {
    // User defined parameters
    fs = samplingFrequency;
    f0 = cutoffFrequency;
    q = qualityFactor;

    // Intermediate variables
    float omega = 2*M_PI*f0/fs;
    float cosOmega = cos(omega);
    float sinOmega = sin(omega);
    float alpha = sinOmega/(2*q);

    // Compute coefficients
    b0 = (1 - cosOmega)/2;
    b1 = 1 - cosOmega;
    b2 = b0;
    a0 = 1 + alpha;
    a1 = -2*cosOmega;
    a2 = 1 - alpha;
}

float LPF::process(float x) {
    float y = (b0/a0)*x + (b1/a0)*x1 + (b2/a0)*x2 - (a1/a0)*y1 - (a2/a0)*y2;
    x2 = x1;
    x1 = x;
    y2 = y1;
    y1 = y;
    return y;
}

#endif
