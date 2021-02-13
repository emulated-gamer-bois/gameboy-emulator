/* This is a temporary helper that reads the tempscreen.pixdata file. tempscreen.pixdata is a 160x144 24-bit bmp file
 * with its header removed leaving only the pixel data. The pixel data is in rgb format (ie 3 bytes per pixel) but
 * since all pixels in the tempscreen file are in grey (r, g and b have the same value) scale and all pixels that
 * will be generated by the emulator will be in grey scale we only need to store the r-component in a texture (one
 * byte per pixel). The shaders will do the rest of the work.
 */

#ifndef LAME_BOY_TEMP_SCREEN_HELPER_H
#define LAME_BOY_TEMP_SCREEN_HELPER_H

#include <stdint.h> // uint8_t
#include <string>   // string
#include <fstream>  // file reading
#include <iostream>

using namespace std;

namespace temporary {
    const static int BYTE_AMOUNT_FILE = 69120;

    uint8_t* importTempscreen();
}

#endif //LAME_BOY_TEMP_SCREEN_HELPER_H
