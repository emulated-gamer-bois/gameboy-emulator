/* This is a temporary helper that reads the tempscreen.pixdata file.
 */

#include "temp_screen_helper.h"

namespace temporary{
    uint8_t* importTempscreen() {
        int texSize = BYTE_AMOUNT_FILE / 3;
        char* buffer = new char[BYTE_AMOUNT_FILE];
        uint8_t* byteArr = new uint8_t[texSize];
        std::ifstream file("tempscreen.pixdata", ios::in | ios::binary);
        if (file.is_open()) {
            file.read(buffer, BYTE_AMOUNT_FILE);
            file.close();
        }
        else {
            return nullptr;
        }

        return (uint8_t*)buffer;
    }
}