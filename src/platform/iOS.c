#include "stdafx.h"

#include <CoreFoundation/CoreFoundation.h>

void InitializePlatform() {
    // recall: iOS gets weird if you call a directory "Resources" in the base of the .app,
    //   so the stuff is copied to a directory called DunGenResources
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) {
        fprintf(stderr, "Problem setting up working directory! Probably nothing will work!\n");
    }
    CFRelease(resourcesURL);
    chdir(path);
    chdir("DunGenResources");
}

void FinalizePlatform() {
}

sds GetShaderPath(const char* shaderName) {
    #if defined(SOKOL_GLES3)
        char* path = "shaders/glsl300es/";
        char* suffix = ".glsl";
    #elif defined(SOKOL_METAL)
        #if TARGET_OS_SIMULATOR
            char* path = "shaders/metal_sim/";
            char* suffix = "metal";
        #else
            char* path = "shaders/metal_ios/";
            char* suffix = ".metallib";
        #endif // TARGET_OS_SIMULATOR
    #endif // defined(SOKOL_GLES3)

    return sdscatfmt(sdsempty(), "%s%s%s", path, shaderName, suffix);;
}
