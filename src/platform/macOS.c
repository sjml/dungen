#include "stdafx.h"

#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>

void InitializePlatform() {
    #if DEBUG
        chdir("../Resources");
    #else
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
        char path[PATH_MAX];
        if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) {
            fprintf(stderr, "Problem setting up working directory! Probably nothing will work!\n");
        }
        CFRelease(resourcesURL);
        chdir(path);
    #endif
}

void FinalizePlatform() {
}
