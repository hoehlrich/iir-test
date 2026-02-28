#ifndef LPF_H
#define LPF_H

#include <cstdio>
#include <cstdint>
#include <fftw3.h>
#include <SFML/Graphics.hpp>

#include "CircularBuffer.hpp"
#include "Spectrogram.hpp"
#include "Recorder.hpp"

#define FFT_SIZE    1024

#define NUM_SECONDS         5
#define NUM_CHANNELS        1
#define TOTAL_FRAMES        NUM_SECONDS*SAMPLE_RATE
#define NUM_SAMPLES         TOTAL_FRAMES*NUM_CHANNELS

#define FUND_FREQ           ((float) ((float) SAMPLE_RATE/FFT_SIZE))

#define WIN_WIDTH   1280
#define WIN_HEIGHT  800
class App {
    public:
        App();
        App(const App&) = delete;
        App& operator=(const App&) = delete;
        ~App();
        void readSamples(const char *filename);
        void run();
        void handleEvents();
    private:
        sf::RenderWindow window;
        fftwf_complex *out;
        Spectrogram spectrogram;
        const uint32_t numBytes = NUM_SAMPLES * sizeof(float);
        float *recordedSamples;
        float *fftIn;
        fftwf_plan p;
        size_t sampleIdx = 0;
        CircularBuffer<float> buf;
        Recorder recorder;
};

App::App()
    : window(sf::VideoMode({WIN_WIDTH, WIN_HEIGHT}), "Spectrogram"),
      out((fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * FFT_SIZE)),
      spectrogram(&window, out, FFT_SIZE, FUND_FREQ, sf::Vector2f(0, 0), sf::Vector2f(WIN_WIDTH, WIN_HEIGHT), sf::Vector2f(-80, 0)),
      buf(FFT_SIZE),
      recorder(FFT_SIZE)
{
    recordedSamples = nullptr;
    fftIn = (float*) fftwf_malloc(sizeof(float) *FFT_SIZE);
    p = fftwf_plan_dft_r2c_1d(FFT_SIZE, fftIn, out, FFTW_ESTIMATE);
}

void App::run() {

    while (window.isOpen()) {
        handleEvents();
        if (recorder.readBlock(fftIn, FFT_SIZE)) {
            fftwf_execute(p);
            window.clear();
            spectrogram.drawBars();
            spectrogram.drawAxis();
            window.display();
        }
    }
}

void App::handleEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            break;
        } else if (const sf::Event::KeyPressed *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::N) {
                if (recordedSamples == nullptr) continue;
                if (sampleIdx + FFT_SIZE >= NUM_SAMPLES) continue;
                for (size_t i = 0; i < FFT_SIZE; i++) {
                    buf.write(recordedSamples[sampleIdx++]);
                }
                buf.readBlock(fftIn, FFT_SIZE);
                fftwf_execute(p);
                spectrogram.clearBars(sf::Color::Black);
                spectrogram.drawBars();
                window.display();
            } else if (keyPressed->code == sf::Keyboard::Key::P) {
                if (recordedSamples == nullptr) continue;
                if (sampleIdx < 2*FFT_SIZE) continue;
                sampleIdx -= 2*FFT_SIZE;
                for (size_t i = 0; i < FFT_SIZE; i++) {
                    buf.write(recordedSamples[sampleIdx++]);
                }
                buf.readBlock(fftIn, FFT_SIZE);
                fftwf_execute(p);
                spectrogram.clearBars(sf::Color::Black);
                spectrogram.drawBars();
                window.display();
            } else if (keyPressed->code == sf::Keyboard::Key::R) {
                recorder.start();
            }
        }
    }
}

void App::readSamples(const char *filename) {
    FILE *fid = fopen(filename, "rb");
    recordedSamples = (float*) malloc(sizeof(float)*numBytes);
    size_t retCode = fread(recordedSamples, sizeof(float), TOTAL_FRAMES, fid);
    if (retCode == TOTAL_FRAMES) {
        std::cout << "Array read " << TOTAL_FRAMES << " frames successfully" << std::endl;
    } else {
        std::cout << "Failed to read \'" << filename << "\' (" << retCode << ")" << std::endl;
    }
}

App::~App() {
    fftwf_destroy_plan(p);
    fftwf_free(fftIn);
    fftwf_free(out);
    free(recordedSamples);
}

#endif
