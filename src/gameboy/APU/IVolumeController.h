//
// Created by David on 2021-03-28.
//

#pragma once

class IVolumeController {
public:
    virtual void setVolume(int source, float volume) = 0;
};