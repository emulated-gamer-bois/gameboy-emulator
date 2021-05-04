
#include "AudioController.h"

AudioController::AudioController(AppSettings& settings):
    settings{settings}
{
    init();
}

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

    alGenBuffers(1, &tmpBuffer);
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
    for(auto i = 0; i < LFSR15_BUFFER_SIZE; i++) {
        xor_res = (lfsr15 ^ (lfsr15 >> 1)) & 1;
        lfsr15 >>= 1;
        lfsr15 |= xor_res << 14;
        noise15bit[i] = lfsr15 & 1 ? 0 : (char) 0xFF;
    }

    for(auto i = 0; i < LFSR7_BUFFER_SIZE; i++) {
        xor_res = (lfsr7 ^ (lfsr7 >> 1)) & 1;
        lfsr7 >>= 1;
        lfsr7 |= xor_res << 6;
        noise7bit[i] = lfsr7 & 1 ? 0 : (char)0xFF;
    }
}

void swap(ALuint *a, ALuint *b) {
    auto tmp = *a;
    *a = *b;
    *b = tmp;
}

void AudioController::playSound(int source, uint8_t *soundData, int size, int sampleRate, float volume) {
    alBufferData(tmpBuffer, AL_FORMAT_MONO8, soundData, size, sampleRate);
    alSourcef(sources[source], AL_GAIN, volume);
    alSourceStop(sources[source]);
    alSourcei(sources[source], AL_BUFFER, tmpBuffer);
    alSourcePlay(sources[source]);
    alSourcei(sources[source], AL_LOOPING, 1);
    swap(&tmpBuffer, buffers + source);
}

/**
 * Plays a square wave sound until stopped
 * @param source index (0 to 3)
 * @param duty (0 = 12.5%, 1 = 25%, 2 = 50%, 3 = 75%)
 * @param frequency the frequency of the sound
 */
void AudioController::playSquare(int source, char duty, ALsizei frequency, float volume) {
    //alSourcei(sources[source], AL_BUFFER, 0);
    alBufferData(
        tmpBuffer,
        AL_FORMAT_MONO8,
        duties[duty],
        SQUARE_SAMPLE_RATE,
        SQUARE_SAMPLE_RATE * frequency);
    alSourcef(sources[source], AL_GAIN, volume);
    alSourceStop(sources[source]);
    alSourcei(sources[source], AL_BUFFER, tmpBuffer);
    alSourcePlay(sources[source]);
    alSourcei(sources[source], AL_LOOPING, 1);
    swap(&tmpBuffer, buffers + source);
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

    alBufferData(
            tmpBuffer,
            AL_FORMAT_MONO8,
            wave,
            32,
            32 * frequency);
    alSourcef(sources[source], AL_GAIN, volume);
    alSourceStop(sources[source]);
    alSourcei(sources[source], AL_BUFFER, tmpBuffer);
    alSourcePlay(sources[source]);
    alSourcei(sources[source], AL_LOOPING, 1);

    delete[] wave;
    swap(&tmpBuffer, buffers + source);
}

void AudioController::playGBWave(int source, std::array<uint8_t, 16> waveForm, ALsizei frequency, float volume) {
    this->playWave(source, waveForm, 131072.0/(2048 - frequency), volume);
}

void AudioController::setVolume(int source, float volume) {
    alSourcef(sources[source], AL_GAIN, volume * settings.masterVolume);
}

void AudioController::playNoise(int source, bool is_7_bit_mode, ALsizei frequency, float volume) {
    if(is_7_bit_mode) {
        playSound(source, noise7bit, LFSR7_BUFFER_SIZE, frequency, volume);
    } else {
        playSound(source, noise15bit, LFSR15_BUFFER_SIZE, frequency, volume);
    }
}

void AudioController::stopSound() {
    for(int i=0;i<N_SOURCES;i++){
        stopSource(i);
    }
}

void AudioController::stepSound(uint8_t i, APUState *state) {
    //1st square
    if (i & 1) {
        if (state->enableSquareA) {
            playGBSquare(0, state->dutySquareA, state->frequencySquareA, state->volumeSquareA * settings.masterVolume);
        } else {
            stopSource(0);
        }
    }

    //2nd square
    if (i & 2) {
        if (state->enableSquareB) {
            playGBSquare(1, state->dutySquareB, state->frequencySquareB, state->volumeSquareB * settings.masterVolume);
        } else {
            stopSource(1);
        }
    }

    //Wave
    if (i & 4) {
        if (state->enableWave) {
            playGBWave(2, state->waveform, state->frequencyWave, state->volumeWave * settings.masterVolume);
        } else {
            stopSource(2);
        }
    }
    //Noise
    if(i & 8) {
        if(state->enableNoise) {
            playNoise(3, state->is7BitMode, state->frequencyNoise, state->volumeNoise * settings.masterVolume);
        } else {
            stopSource(3);
        }
    }
}
