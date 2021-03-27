//
// Created by riddarvid on 3/25/21.
//

#ifndef LAME_BOY_AUDIOCONTROLLER_H
#define LAME_BOY_AUDIOCONTROLLER_H

#include <AL/alc.h>
#include <AL/al.h>
#include <iostream>
#include <array>

class AudioController {
public:
    ~AudioController();
    void init();
    void playSound(int source, char *soundData, int size, int sampleRate);
    void playSquare(int source, char duty, ALsizei frequency, float volume);
    void playGBSquare(int source, char duty, unsigned short frequency, float volume);
    void stopSource(int source);
    void playGBWave(int source, std::array<uint8_t, 16> waveForm, ALsizei frequency);
    void playWave(int source, std::array<uint8_t, 16> waveForm, ALsizei frequency);
private:

    ALCdevice* device;
    ALCcontext* context;

    const static int N_SOURCES = 4;

    ALuint buffers[N_SOURCES];
    ALuint sources[N_SOURCES];

    const static int SQUARE_SAMPLE_RATE = 32;
    unsigned char duties[N_SOURCES][SQUARE_SAMPLE_RATE];
};


#endif //LAME_BOY_AUDIOCONTROLLER_H
