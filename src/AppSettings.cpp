#include "AppSettings.h"

void AppSettings::setPlaySpeed(double speedMultiplier) {
    this->playSpeed = 1000.f / (LCD_REFRESH_RATE*speedMultiplier);

}
