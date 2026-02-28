#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <fftw3.h>
#include <cmath>
#include <sstream>

class Spectrogram {
    public:
      Spectrogram(sf::RenderWindow *_window, fftwf_complex *_dft,
                  uint32_t _fftSize, uint32_t _fundFreq, sf::Vector2f origin,
                  sf::Vector2f _size, sf::Vector2f _dBRange);
      Spectrogram(const Spectrogram& OTHER) = delete;
      void drawBars();
      void clearBars(sf::Color color);
      void drawAxis();
    private:
        sf::RenderWindow *window;
        fftwf_complex *dft;
        uint32_t fftSize, fundFreq;
        sf::Vector2f origin;
        sf::Vector2f size; // width, height
        sf::Vector2f dBRange; // min, max
        const float margin = 50;
        const sf::Vector2u numLabels = sf::Vector2u({5, 5});
        sf::Font font;
};

Spectrogram::Spectrogram(sf::RenderWindow *_window, fftwf_complex *_dft,
                         uint32_t _fftSize, uint32_t _fundFreq,
                         sf::Vector2f _origin, sf::Vector2f _size,
                         sf::Vector2f _dBRange) {
    window = _window;
    dft = _dft;
    fftSize = _fftSize;
    fundFreq = _fundFreq;
    origin = _origin;
    size = _size;
    dBRange = _dBRange;
    if(!font.openFromFile("/usr/share/fonts/liberation/LiberationMono-Regular.ttf")) {};
}

void Spectrogram::drawAxis() {
    sf::Text text(font);
    const uint32_t fontSize = 14;
    text.setCharacterSize(fontSize);

    // Frequency axis
    float xLabelStep = (size.x - 2*margin)/(numLabels.x - 1);
    float freqStep = ((float) fundFreq * fftSize)/(numLabels.x - 1);
    for (size_t i = 0; i < numLabels.x; i++) {
        float freq = freqStep*i;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1);
        if (freq > 1000) {
            ss << (freq / 1000) << " kHz";
        } else {
            ss << freq << " Hz";
        }

        text.setString(ss.str());
        float xpos = i == 0 ? margin : i*xLabelStep;
        text.setPosition(sf::Vector2f({xpos, size.y - margin}));
        window->draw(text);
    }

    // dB axis
    float yLabelStep = (size.y - 2*margin)/(numLabels.y - 1);
    float dBStep = ((float) dBRange.y - dBRange.x)/(numLabels.y - 1);
    float xpos = (margin - 3*fontSize)/2;
    for (size_t i = 0; i < numLabels.y; i++) {
        text.setString(std::to_string((int32_t) (dBRange.x + dBStep*i)) + "dB");
        float ypos = i == 0 ? size.y - margin - fontSize : size.y - i*yLabelStep - margin/2 - fontSize;
        text.setPosition(sf::Vector2f({xpos, ypos}));
        window->draw(text);
    }
}

void Spectrogram::clearBars(sf::Color color) {
    sf::RectangleShape rectangle;
    rectangle.setPosition(origin + sf::Vector2f({margin, margin}));
    rectangle.setSize(size - sf::Vector2f({2*margin, 2*margin}));
    rectangle.setFillColor(color);
    window->draw(rectangle);
}

void Spectrogram::drawBars() {
    size_t numBars = (fftSize/2) + 1;
    float barWidth = (size.x - 2*margin)/numBars;
    float maxHeight = size.y - 2*margin;
    sf::VertexArray bars(sf::PrimitiveType::Triangles, numBars*6);

    for (size_t i = 0; i < numBars; i++) {
        double magnitude = sqrt(pow(dft[i][0], 2) + pow(dft[i][1], 2));
        double dB = 20*log10((2*magnitude)/fftSize);
        if (dB < dBRange.x) dB = dBRange.x; // clamp to min
        if (dB > dBRange.y) dB = dBRange.y; // clamp to max
        double percentage = (dB - dBRange.x)/(dBRange.y - dBRange .x);

        sf::Vector2f p0 = origin + sf::Vector2f(i*barWidth + margin, size.y - margin);
        sf::Vector2f p1 = p0 - sf::Vector2f(0, maxHeight*percentage);
        sf::Vector2f p2 = p0 + sf::Vector2f(barWidth, 0);
        sf::Vector2f p3 = p1 + sf::Vector2f(barWidth, 0);

        bars[6*i + 0].position = p0;
        bars[6*i + 1].position = p1;
        bars[6*i + 2].position = p2;
        bars[6*i + 3].position = p1;
        bars[6*i + 4].position = p2;
        bars[6*i + 5].position = p3;

        bars[6*i + 0].color = sf::Color::White;
        bars[6*i + 1].color = sf::Color::White;
        bars[6*i + 2].color = sf::Color::White;
        bars[6*i + 3].color = sf::Color::White;
        bars[6*i + 4].color = sf::Color::White;
        bars[6*i + 5].color = sf::Color::White;
    }

    window->draw(bars);
}

#endif
