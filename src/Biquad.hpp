#ifndef BIQUAD_H
#define BIQUAD_H

#include <cstdint>

class ABiquad {
    public:
        ABiquad(uint32_t fs, uint32_t f0, float q);
        ~ABiquad();
    private:
        uint32_t fs, f0;
        float q;
        float a0, a1, a2, b0, b1, b2;
};

#endif
