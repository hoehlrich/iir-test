# IIR-test

Program that records audio input and filters out a fan noise using an IIR
filter.

1. Acquisition Phase (Input)
- PortAudio/RtAudio for input via the soundcard
- Move samples from the hardware buffer into a circular buffer
- Once the circular buffer has enough samples copy a frame into an FFT Input Array

2. Conditioning (Time domain)
- Windowing to prevent specrtal leakage
- Normalization: cast to double normalized to +- 1.0 for FFTW

3. Transformation and extraction
- Use FFTW to transform N real samples into N/2 + 1 complex frequency bins
- For each complex bin ```X[k]``` calculate the magnitude
- Convert magnitude to dB
- Frequency mapping: calculte the frequency for each bin index k

TODO:
- [x] Follow along a Audio EQ Cookbook example calculating the coefficients for a LPF
