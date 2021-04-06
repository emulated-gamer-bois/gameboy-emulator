//
// Created by riddarvid on 3/25/21.
//

#include "AudioController.h"
#include "../gameboy/APU/APUState.h"

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

    uint16_t lfsr7 = 0x7F;
    uint16_t lfsr15 = 0x7FFF;
    auto xor_res = 0;
    for(auto i = 0; i < NOISE_BUFFER_SIZE; i++) {
        xor_res = (lfsr15 ^ (lfsr15 >> 1)) & 1;
        lfsr15 >>= 1;
        lfsr15 |= xor_res << 14;
        noise15bit[i] = lfsr15 & 1 ? 0 : (char)0xFF;

        xor_res = (lfsr7 ^ (lfsr7 >> 1)) & 1;
        lfsr7 >>= 1;
        lfsr7 |= xor_res << 6;
        noise7bit[i] = lfsr7 & 1 ? 0 : (char)0xFF;
    }
}

void AudioController::playSound(int source, uint8_t *soundData, int size, int sampleRate, float volume) {
    alSourcei(sources[source], AL_BUFFER, 0);
    alBufferData(buffers[source], AL_FORMAT_MONO8, soundData, size, sampleRate);
    alSourcef(sources[source], AL_GAIN, volume);
    alSourcei(sources[source], AL_BUFFER, buffers[source]);
    alSourcei(sources[source], AL_LOOPING, 1);
    alSourcePlay(sources[source]);
}

/**
 * Plays a square wave sound until stopped
 * @param source index (0 to 3)
 * @param duty (0 = 12.5%, 1 = 25%, 2 = 50%, 3 = 75%)
 * @param frequency the frequency of the sound
 */
void AudioController::playSquare(int source, char duty, ALsizei frequency, float volume) {
    alSourcei(sources[source], AL_BUFFER, 0);
    alBufferData(
        buffers[source],
        AL_FORMAT_MONO8,
        duties[duty],
        SQUARE_SAMPLE_RATE,
        SQUARE_SAMPLE_RATE * frequency);
    alSourcef(sources[source], AL_GAIN, volume);
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
void AudioController::playGBSquare(int source, char duty, unsigned short frequency, float volume) {
    this->playSquare(source, duty, 131072.0/(2048 - frequency), volume);
}

void AudioController::stopSource(int source) {
    alSourceRewind(sources[source]);
}

void AudioController::playWave(int source, std::array<uint8_t, 16> waveForm, ALsizei frequency, float volume) {
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
    alSourcef(sources[source], AL_GAIN, volume);
    alSourcei(sources[source], AL_BUFFER, buffers[source]);
    alSourcei(sources[source], AL_LOOPING, 1);
    alSourcePlay(sources[source]);

    delete[] wave;
}

void AudioController::playGBWave(int source, std::array<uint8_t, 16> waveForm, ALsizei frequency, float volume) {
    this->playWave(source, waveForm, 131072.0/(2048 - frequency), volume);
}

void AudioController::setVolume(int source, float volume) {
    alSourcef(sources[source], AL_GAIN, volume);
}

AudioController::AudioController() {
    init();
}

void AudioController::playNoise(int source, bool is_7_bit_mode, ALsizei frequency, float volume) {
    playSound(source, is_7_bit_mode ? noise7bit : noise15bit, NOISE_BUFFER_SIZE, frequency, volume);
}

void AudioController::stopSound() {
    for(int i=0;i<N_SOURCES;i++){
        stopSource(i);
    }
}

void AudioController::stepSound(uint8_t i, APUState *state) {

    //1st square
    if (i & 1) {
        stopSource(0);
        if (state->enable_square_a) {
            playGBSquare(0, state->duty_square_a, state->frequency_square_a, state->volume_square_a);
        }
    }

    //2nd square
    if (i & 2) {
        stopSource(1);
        if (state->enable_square_b) {
            playGBSquare(1, state->duty_square_b, state->frequency_square_b, state->volume_square_b);
        }
    }

    //Wave
    if (i & 4) {
        stopSource(2);
        if (state->enable_wave) {
            playGBWave(2, state->waveform_wave, state->frequency_wave, state->volume_wave);
        }
    }
    //Noise
    if(i & 8) {
        stopSource(3);
        if(state->enable_noise) {
            playNoise(3, state->is_7_bit_mode, state->frequency_noise, state->volume_noise);
        }
    }
}