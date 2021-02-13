/* This is a temporary helper that reads the tempscreen.pixdata file.
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
