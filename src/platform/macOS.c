#include "stdafx.h"

#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>

#include "../infrastructure/log.h"

void InitializePlatform() {
    #if DEBUG
        if (chdir("./Resources") != 0) {
            LogErr("ERROR: Could not find Resources directory. Exiting.\n");
            exit(EXIT_FAILURE);
        }
    #else
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
        char path[PATH_MAX];
        if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) {
            LogErr("Problem setting up working directory! Probably nothing will work!\n");
        }
        CFRelease(resourcesURL);
        chdir(path);
    #endif
}

void FinalizePlatform() {
}

sds GetShaderPath(const char* shaderName) {
    #if defined(SOKOL_GLCORE33)
        char* path = "shaders/glsl330/";
        char* suffix = ".glsl";
    #elif defined(SOKOL_METAL)
        char* path = "shaders/metal_macos/";
        char* suffix = ".metallib";
    #endif // defined(SOKOL_GLCORE33)

    return sdscatfmt(sdsempty(), "%s%s%s", path, shaderName, suffix);;
}
