#include "../stdafx.h"

#include <unistd.h>

#include "../infrastructure/images.h"
#include "../infrastructure/log.h"

void InitializePlatform() {
    if (chdir("./Resources") != 0) {
        LogErr("ERROR: Could not find Resources directory. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    unsigned int smallDataLength;
    int smallImageWidth, smallImageHeight;
    const unsigned char* small = GetRawImageData("images/appicons/128.png", &smallDataLength, &smallImageWidth, &smallImageHeight);
    unsigned int mediumDataLength;
    int mediumImageWidth, mediumImageHeight;
    const unsigned char* medium = GetRawImageData("images/appicons/256.png", &mediumDataLength, &mediumImageWidth, &mediumImageHeight);
    unsigned int largeDataLength;
    int largeImageWidth, largeImageHeight;
    const unsigned char* large = GetRawImageData("images/appicons/512.png", &largeDataLength, &largeImageWidth, &largeImageHeight);

    const sapp_icon_desc icon_desc = {
        .images = {
            { .width = 128, .height = 128, .pixels = { .ptr = small,  .size = smallDataLength  * sizeof(unsigned char)} },
            { .width = 256, .height = 256, .pixels = { .ptr = medium, .size = mediumDataLength * sizeof(unsigned char)} },
            { .width = 512, .height = 512, .pixels = { .ptr = large,  .size = largeDataLength  * sizeof(unsigned char)} },
        }
    };
    sapp_set_icon(&icon_desc);
}

void FinalizePlatform() {
}

sds GetShaderPath(const char* shaderName) {
    char* path = "shaders/glsl330/";
    char* suffix = ".glsl";

    return sdscatfmt(sdsempty(), "%s%s%s", path, shaderName, suffix);;
}
