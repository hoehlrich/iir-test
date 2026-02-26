#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <memory>

template <typename T>
class CircularBuffer {
    public:
        CircularBuffer(uint32_t capacity);
        void write(T value);
        T read();
        std::unique_ptr<T[]> readBlock(uint32_t size);
        void writeBlock(std::unique_ptr<T[]> block, uint32_t size);
        void readToFFT(T *fftIn, uint32_t N);
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
        start = (start + 1) % capacity; // Overflow
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
std::unique_ptr<T[]> CircularBuffer<T>::readBlock(uint32_t size) {
    std::unique_ptr<T[]> block = std::make_unique<T[]>(size);
    for (uint32_t i = 0; i < size; i++) {
        block[i] = this->read();
    }
}

template <typename T>
void CircularBuffer<T>::writeBlock(std::unique_ptr<T[]> block, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        data[end] = block[i];
        end = (end + 1) % capacity;
    }

    // Change currSize once
    if (currSize + size < capacity) {
        currSize += size;
    } else {
        currSize = capacity;
        start = end;    // update start when overflows
    }
}

template <typename T>
void CircularBuffer<T>::readToFFT(T *fftIn, uint32_t N) {
    uint32_t first = N <= (capacity - start) ? N : capacity - start; // num elements to read from start
    uint32_t second = N - first; // num elements to read from 0

    std::memcpy(fftIn, &data[start], sizeof(T) * first);

    if (second > 0)
        std::memcpy(&fftIn[first], &data[0], sizeof(T) * second);
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

