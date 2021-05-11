#pragma once

class IVolumeController {
public:
  /**
   * Sets the volume of the specified source
   * @param source the source which will be affected
   * @param volume the new volume (0.0 to 1.0)
   */
    virtual void setVolume(int source, float volume) = 0;
};