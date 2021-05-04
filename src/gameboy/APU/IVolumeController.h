#pragma once

class IVolumeController {
public:
    virtual void setVolume(int source, float volume) = 0;
};