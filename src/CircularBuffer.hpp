#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <memory>

template <typename T>
class CircularBuffer {
    public:
        CircularBuffer(uint32_t capacity);
        void write(T value);
        T read();
        T *readBlock(uint32_t size);
        int writeBlock(T *block);
        void print();
    private:
        std::unique_ptr<T[]> data;
        uint32_t capacity;
        uint32_t start; // read index
        uint32_t end;   // write index
        uint32_t currSize;
};

template <typename T>
CircularBuffer<T>::CircularBuffer(uint32_t bufCapacity) {
    capacity = bufCapacity;
    data = std::make_unique<T[]>(capacity);
    start = end = currSize = 0;
}

template <typename T>
void CircularBuffer<T>::write(T value) {
    data[end] = value;
    end = (end + 1) % capacity;

    if (currSize < capacity) {
        currSize++;
    } else {
        start = (start + 1) % capacity;
    }
}

template <typename T>
T CircularBuffer<T>::read() {
    if (currSize == 0) return T(0); // Underflow

    T value = data[start];
    start = (start + 1) % capacity;
    currSize--;
    return value;
}

template <typename T>
void CircularBuffer<T>::print() {
    std::cout << '|';
    for (uint32_t i = 0; i < capacity; i++) {
        std::cout << std::setw(4) << data[i] << '|';
    }
    std::cout << std::endl << '|';
    for (uint32_t i = 0; i < capacity; i++) {
        std::cout << std::setw(4);
        if (i == start && i == end) {
            std::cout << "s e";
        } else if (i == start) {
            std::cout << 's';
        } else if (i == end) {
            std::cout << 'e';
        } else {
            std::cout << ' ';
        }
        std::cout << '|';
    }
    std::cout << std::endl;
}

#endif

