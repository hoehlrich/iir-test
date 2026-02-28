#ifndef RECORDER_H
#define RECORDER_H

#include <atomic>
#include <cstdint>
#include "CircularBuffer.hpp"
#include "portaudio.h"

#define SAMPLE_RATE         16000
#define FRAMES_PER_BUFFER   512
#define NUM_CHANNELS        1

class Recorder {
    public:
        Recorder(uint32_t fftSize);
        ~Recorder();

        bool start();
        void stop();
        int readBlock(float* outputBuffer, uint32_t framesToRead);
    private:
        PaStream *stream;
        PaStreamParameters inputParameters;
        uint32_t fftSize;
        CircularBuffer<float> buf;
        std::atomic<bool> paused{false};
        std::atomic<bool> dataReady{false};

        static int pAudioCallback(const void *inputBuffer, void *outputBuffer,
                                  uint64_t framesPerBuffer,
                                  const PaStreamCallbackTimeInfo *timeInfo,
                                  PaStreamCallbackFlags statusFlags,
                                  void *userData);
};

Recorder::Recorder(uint32_t _fftSize) 
    : fftSize(_fftSize),
      buf(_fftSize)
{ }

Recorder::~Recorder() {
    this->stop();
}

bool Recorder::start() {
    PaError err = Pa_Initialize();
    if (err != paNoError) return false;

    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    std::cout << "here" << std::endl;
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr, "Error: No default input device.\n");
        return false;
    }


    err = Pa_OpenStream(&stream, &inputParameters,
                        NULL, SAMPLE_RATE,
                        FRAMES_PER_BUFFER, paClipOff,
                        &Recorder::pAudioCallback, this);

    err = Pa_StartStream(stream);
    if (err != paNoError) return false;

    return true;
}

void Recorder::stop() {
    if (stream != nullptr) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        stream = nullptr;
    }
}

int Recorder::readBlock(float* outputBuffer, uint32_t framesToRead) {
    uint32_t currSize = buf.getCurrSize();
    if (currSize >= framesToRead) {
        buf.readBlock(outputBuffer, framesToRead);
        return framesToRead;
    }
    return 0;
}

int Recorder::pAudioCallback(
        const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo *timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData
        )
{
    Recorder *data = static_cast<Recorder*>(userData);
    const float *rptr = static_cast<const float*>(inputBuffer);

    (void) outputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;

    unsigned long framesToCalc = data->paused ? 0: framesPerBuffer;

    if (inputBuffer == NULL) {
        for (unsigned long i = 0; i < framesToCalc; i++) {
            data->buf.write(0.0f);
            if (NUM_CHANNELS == 2) data->buf.write(0.0f);
        }
    } else {
        for (unsigned long i = 0; i < framesToCalc; i++) {
            data->buf.write(*rptr++);
            if (NUM_CHANNELS == 2) data->buf.write(*rptr++);
        }
    }
    return paContinue;
}

#endif
