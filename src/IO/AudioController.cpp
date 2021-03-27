//
// Created by riddarvid on 3/25/21.
//

#include "AudioController.h"

AudioController::~AudioController() {
    alDeleteSources(4, sources);
    alDeleteBuffers(4, buffers);
    alcMakeContextCurrent(context);
    alcDestroyContext(context);

    if(!alcCloseDevice(device))
        std::cerr << "Field to close device\n";
}

void AudioController::init() {
    device = alcOpenDevice(nullptr);
    ALCenum error;

    if(!device) {
        std::cerr << "Failed to open device\n";
        return;
    }

    context = alcCreateContext(device, nullptr);
    error = alcGetError(device);

    if(error || !context) {
        std::cerr << "ERROR: Could not create audio context" << std::endl;
        return;
    }

    auto contextMadeCurrent = alcMakeContextCurrent(context);
    error = alcGetError(device);

    if(error || contextMadeCurrent != ALC_TRUE) {
        std::cerr << "ERROR: Could not make audio context current" << std::endl;
        return;
    }

    alGenBuffers(N_SOURCES, buffers);
    alGenSources(N_SOURCES, sources);

    for(auto i = 0; i < N_SOURCES; i++) {
        alSourcef(sources[i], AL_PITCH, 1);
        alSourcef(sources[i], AL_GAIN, 1.0f);
        alSource3f(sources[i], AL_POSITION, 0, 0, 0);
        alSource3f(sources[i], AL_VELOCITY, 0, 0, 0);
        alSourcei(sources[i], AL_LOOPING, AL_FALSE);
    }

    const unsigned char dutyPatterns[4] = {0x01, 0x81, 0x87, 0x7E};
    const int stepLength = SQUARE_SAMPLE_RATE / 8;
    unsigned char value;
    int j;

    for(int i = 0; i < 4; i++) {
        j = 0;
        while(j < SQUARE_SAMPLE_RATE) {
            value = ((dutyPatterns[i] >> (7 - (j / stepLength))) & 1) * 0xFF;
            do {
                duties[i][j] = value;
                j++;
            } while(j % stepLength);
        }
    }
}

void AudioController::playSound(int source, char *soundData, int size, int sampleRate) {
    alBufferData(buffers[source], AL_FORMAT_MONO8, soundData, size, sampleRate);
    alSourcei(sources[source], AL_BUFFER, buffers[source]);
    alSourcePlay(sources[source]);
}

/**
 * Plays a square wave sound until stopped
 * @param source index (0 to 3)
 * @param duty (0 = 12.5%, 1 = 25%, 2 = 50%, 3 = 75%)
 * @param frequency the frequency of the sound
 */
void AudioController::playSquare(int source, char duty, ALsizei frequency) {
    alSourcei(sources[source], AL_BUFFER, 0);
    alBufferData(
        buffers[source],
        AL_FORMAT_MONO8,
        duties[duty],
        SQUARE_SAMPLE_RATE,
        SQUARE_SAMPLE_RATE * frequency);
    alSourcei(sources[source], AL_BUFFER, buffers[source]);
    alSourcei(sources[source], AL_LOOPING, 1);
    alSourcePlay(sources[source]);
}

/**
 * Plays a square wave sound until stopped
 * @param source index (0 to 3)
 * @param duty (0 = 12.5%, 1 = 25%, 2 = 50%, 3 = 75%)
 * @param frequency the values read from the F bits
 */
void AudioController::playGBSquare(int source, char duty, unsigned short frequency) {
    this->playSquare(source, duty, 131072.0/(2048 - frequency));
}

void AudioController::stopSource(int source) {
    alSourceRewind(sources[source]);
}

void AudioController::playWave(int source, std::array<uint8_t, 16> waveForm, ALsizei frequency) {
    auto wave = new char[32];
    uint8_t form;
    for(auto i = 0; i < 16; i++) {
        form = waveForm[i] & 0xF;
        wave[i*2] = form * form;
        form = (waveForm[i] & 0xF0) >> 4;
        wave[i*2+1] = form * form;
    }

    alSourcei(sources[source], AL_BUFFER, 0);
    alBufferData(
            buffers[source],
            AL_FORMAT_MONO8,
            wave,
            32,
            32 * frequency);
    alSourcei(sources[source], AL_BUFFER, buffers[source]);
    alSourcei(sources[source], AL_LOOPING, 1);
    alSourcePlay(sources[source]);

    delete[] wave;
}

void AudioController::playGBWave(int source, std::array<uint8_t, 16> waveForm, ALsizei frequency) {
    this->playWave(source, waveForm, 131072.0/(2048 - frequency));
}
