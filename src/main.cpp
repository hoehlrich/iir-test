#include <cctype>
#include <cstdint>
#include <iostream>
#include <fftw3.h>
#include <SFML/Graphics.hpp>

#include "CircularBuffer.hpp"
#include "App.hpp"

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
    App app = App();
    app.readSamples("recorded.raw");
    app.run();
}
