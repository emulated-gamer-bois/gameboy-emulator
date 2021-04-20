//
// Created by riddarvid on 3/25/21.
//

#ifndef LAME_BOY_AUDIOCONTROLLER_H
#define LAME_BOY_AUDIOCONTROLLER_H

#include <AL/alc.h>
#include <AL/al.h>
#include <iostream>
#include <array>
#include "../gameboy/APU/IVolumeController.h"
#include "../gameboy/APU/APUState.h"
#include "../application/AppSettings.h"



class AudioController : public IVolumeController {
public:
    explicit AudioController(AppSettings& settings);
    ~AudioController();
    void init();
    void playSound(int source, uint8_t *soundData, int size, int sampleRate, float volume);
    void playSquare(int source, char duty, ALsizei frequency, float volume);
    void playGBSquare(int source, char duty, unsigned short frequency, float volume);
    void playGBWave(int source, std::array<uint8_t, 16> waveForm, ALsizei frequency, float volume);
    void playWave(int source, std::array<uint8_t, 16> waveForm, ALsizei frequency, float volume);
    void setVolume(int source, float volume);
    void playNoise(int source, bool is_7_bit_mode, ALsizei frequency, float volume);
    void stopSound();
    void stepSound(uint8_t i, APUState *state);

private:
    const static int N_SOURCES = 4;
    const static int SQUARE_SAMPLE_RATE = 32;
    const static int LFSR7_BUFFER_SIZE = 0x7F;
    const static int LFSR15_BUFFER_SIZE = 0x7FFF;

    ALCdevice* device;
    ALCcontext* context;
    AppSettings& settings;

    ALuint tmpBuffer;
    ALuint buffers[N_SOURCES];
    ALuint sources[N_SOURCES];

    unsigned char noise15bit[LFSR15_BUFFER_SIZE];
    unsigned char noise7bit[LFSR7_BUFFER_SIZE];
    unsigned char duties[N_SOURCES][SQUARE_SAMPLE_RATE];

    void stopSource(int source);
};


#endif //LAME_BOY_AUDIOCONTROLLER_H
