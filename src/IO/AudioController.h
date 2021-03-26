//
// Created by riddarvid on 3/25/21.
//

#ifndef LAME_BOY_AUDIOCONTROLLER_H
#define LAME_BOY_AUDIOCONTROLLER_H

#include <AL/alc.h>
#include <AL/al.h>
#include <iostream>

class AudioController {
public:
    ~AudioController();
    void init();
    void playSound(int source, char *soundData, int size, int sampleRate);
    void playSquare(int source, char duty, ALsizei frequency);
    void playGBSquare(int source, char duty, unsigned short frequency);
    void stopSource(int source);

private:
    ALCdevice* device;
    ALCcontext* context;

    ALuint buffers[4];
    ALuint sources[4];

    const static int SQUARE_SAMPLE_RATE = 32;
    unsigned char duties[4][SQUARE_SAMPLE_RATE];
};


#endif //LAME_BOY_AUDIOCONTROLLER_H
