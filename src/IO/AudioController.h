#pragma once

#include <AL/alc.h>
#include <AL/al.h>
#include <iostream>
#include <array>
#include "../gameboy/APU/IVolumeController.h"
#include "../gameboy/APU/APUState.h"
#include "../application/AppSettings.h"


/**
 * The AudioController is what plays all sounds from the emulator. It supports a set of functions which can play
 * sounds similar to the original Game Boy
 */
class AudioController : public IVolumeController {
public:
    /**
     * Connects to audio device, and creates buffers and sources.
     * Also creates audio data buffers used in playSquare and playNoise.
     *
     * @param settings audio volume strategy, givens the master volume of the application
     */
    explicit AudioController(AppSettings& settings);

    /**
     * Disconnects from audio device and deletes all buffers and sources
     */
    ~AudioController();

    /**
     * Plays any sound with the given audio data
     *
     * @param source is a value from 0 to 3, specifies which source to use
     * @param soundData has all the audio samples which will be played, 0x00 == -1 .. 0xFF == 1
     * @param size is the length of the soundData array
     * @param sampleRate is how many samples to play per second
     * @param volume specifies a value from 0.0 to 1.0, i.e no sound to max volume
     */
    void playSound(int source, uint8_t *soundData, int size, int sampleRate, float volume);

    /**
     * Plays square wave according to specified frequency and duty
     *
     * @param source is a value from 0 to 3, specifies which source to use
     * @param duty defines waveform (0b00 == _______‾, 0b01 == ‾______‾, 0b10 == ‾____‾‾‾, 0b11 == _‾‾‾‾‾‾_)
     * @param frequency value read from the NRx4 and NRx3 registers in the Game Boy
     * @param volume specifies a value from 0.0 to 1.0, i.e no sound to max volume
     */
    void playGBSquare(int source, char duty, unsigned short frequency, float volume);

    /**
     * Plays audio with custom waveform
     *
     * @param source is a value from 0 to 3, specifies which source to use
     * @param waveForm is 32 4-bit samples, 0x0 == -1, 0xF == 1
     * @param frequency value read from the NRx4 and NRx3 registers in the Game Boy
     * @param volume specifies a value from 0.0 to 1.0, i.e no sound to max volume
     */
    void playGBWave(int source, std::array<uint8_t, 16> waveForm, ALsizei frequency, float volume);

    /**
     * Plays noise audio for drums, base or other effects
     * 7 bit LFSR results in a loop after 0x7F samples and 15 bit LFSR loops after 0x7FFF samples. 7 bit LFSR
     * therefore make a more instrument-like sound.
     *
     * @param source is a value from 0 to 3, specifies which source to use
     * @param is_7_bit_mode if true the LFSR is 7 bits long, otherwise it is 15 bits
     * @param frequency calculated from the NR43 register
     * @param volume specifies a value from 0.0 to 1.0, i.e no sound to max volume
     */
    void playNoise(int source, bool is_7_bit_mode, ALsizei frequency, float volume);

    /**
     * Sets the volume of a specific source
     *
     * @param source is a value from 0 to 3, specifies which source to use
     * @param volume specifies a value from 0.0 to 1.0, i.e no sound to max volume
     */
    void setVolume(int source, float volume) override;

    /**
     * Stops all sources from playing
     */
    void stopSound();

    /**
     * Updates all audio sources according to the new APU state
     *
     * @param readyToPlay what sources should update their sound, bit x -> source x should change sound
     * @param state the new audio state which shows what source should be played with what settings
     */
    void stepSound(uint8_t readyToPlay, APUState *state);

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

    void init();
    void stopSource(int source);
    void playSquare(int source, char duty, ALsizei frequency, float volume);
    void playWave(int source, std::array<uint8_t, 16> waveForm, ALsizei frequency, float volume);
};


