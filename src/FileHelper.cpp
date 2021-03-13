#include "FileHelper.h"

#include <stdlib.h>
#include <nfd.h>

std::string FileHelper::openDialog() {
    std::string filePath = "";

    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog( NULL, NULL, &outPath );

    if (result == NFD_OKAY) {
        filePath = outPath;
        free(outPath);
    }

    return filePath;
}