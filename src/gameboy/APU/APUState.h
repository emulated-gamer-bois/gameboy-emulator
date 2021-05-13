#pragma once

/**
 * Stores the state of all audio sources
 */
struct APUState {
    bool enableSquareA;
    uint8_t dutySquareA;
    uint16_t frequencySquareA;
    float volumeSquareA;

    bool enableSquareB;
    uint8_t dutySquareB;
    uint16_t frequencySquareB;
    float volumeSquareB;

    bool enableWave;
    std::array<uint8_t, 16> waveform;
    uint16_t frequencyWave;
    float volumeWave;

    bool enableNoise;
    int frequencyNoise;
    float volumeNoise;
    bool is7BitMode;
};
