#include <cctype>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <fftw3.h>
#include <SFML/Graphics.hpp>

#include "CircularBuffer.hpp"
#include "Spectrogram.hpp"

#define FFT_SIZE    1024

#define SAMPLE_RATE         16000
#define FRAMES_PER_BUFFER   512
#define NUM_SECONDS         5
#define NUM_CHANNELS        1
#define FUND_FREQ           ((double) ((double) SAMPLE_RATE/FFT_SIZE))

#define WIN_WIDTH   1280
#define WIN_HEIGHT  800

typedef float SAMPLE;

using namespace std;

void bufferTest() {
    CircularBuffer<uint32_t> buf = CircularBuffer<uint32_t>(8);
    char c;
    while (true) {
        buf.print();
        cin >> c;
        if (isdigit(c)) {
            buf.write(c - '0');
            cout << "Wrote \'" << c << "\' to buffer" << endl;
        } else if (c == 'r') {
            uint32_t result = buf.read();
            cout << "Read \'" << result << "\' from buffer" << endl;
        } else if (c == 'x') {
            break;
        } else {
            cout << "Input \'" << c << "\' not recognized" << endl;
        }
    }
}

int main() {
    // Read recorded input
    FILE *fid = fopen("recorded.raw", "rb");
    size_t totalFrames = NUM_SECONDS * SAMPLE_RATE;
    uint32_t numSamples = totalFrames * NUM_CHANNELS;
    uint32_t numBytes = numSamples * sizeof(SAMPLE);
    SAMPLE *recordedSamples = (SAMPLE *) malloc(numBytes);
    size_t retCode = fread(recordedSamples, sizeof(SAMPLE), totalFrames, fid);
    if (retCode == totalFrames) {
        cout << "Array read " << totalFrames << " frames successfully" << endl;
    }

    // FFTW initialize
    double *in = (double*) fftw_malloc(sizeof(double) * FFT_SIZE);
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFT_SIZE);
    fftw_plan p = fftw_plan_dft_r2c_1d(FFT_SIZE, in, out, FFTW_ESTIMATE);

    // Execute fft

    sf::RenderWindow window(sf::VideoMode({WIN_WIDTH, WIN_HEIGHT}), "Spectrogram");

    Spectrogram spectrogram =
        Spectrogram(&window, out, FFT_SIZE, FUND_FREQ, sf::Vector2f(0, 0),
                    sf::Vector2f(WIN_WIDTH, WIN_HEIGHT), sf::Vector2f(-80, 0));

    spectrogram.drawAxis();

    size_t sampleIdx = 0;
    CircularBuffer<double> buf = CircularBuffer<double>(FFT_SIZE);
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                break;
            } else if (const sf::Event::KeyPressed *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::N) {
                    if (sampleIdx + FFT_SIZE >= numSamples) continue;
                    for (size_t i = 0; i < FFT_SIZE; i++) {
                        buf.write(recordedSamples[sampleIdx++]);
                    }
                    buf.readToFFT(in, FFT_SIZE);
                    fftw_execute(p);
                    spectrogram.clearBars(sf::Color::Black);
                    spectrogram.drawBars();
                    window.display();
                }
            }
        }
    }

    fftw_destroy_plan(p);
    fftw_free(in); fftw_free(out);
}
