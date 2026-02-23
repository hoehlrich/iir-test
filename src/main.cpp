#include "CircularBuffer.hpp"
#include <cctype>
#include <cstdint>
#include <iostream>

using namespace std;

int main() {
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
        }
    }
    buf.print();
}
