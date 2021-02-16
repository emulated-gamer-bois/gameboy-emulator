#include "temp_screen_helper.h"

namespace temporary{
    uint8_t* importTempscreen() {
        int texSize = BYTE_AMOUNT_FILE / 3;
        char* buffer = new char[BYTE_AMOUNT_FILE]; // tempscreen is in rgb format
        uint8_t* byteArr = new uint8_t[texSize]; // texture is in r format

        std::ifstream file("tempscreen.pixdata", ios::in | ios::binary);
        if (file.is_open()) {
            file.read(buffer, BYTE_AMOUNT_FILE);
            file.close();
        }
        else {
            return nullptr;
        }

        for (int i = 0; i < texSize; i += 1) {
            byteArr[i] = buffer[i * 3];
        }

        delete[] buffer;

        return byteArr;
    }
}