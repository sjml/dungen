#include "stdafx.h"

#include <CoreFoundation/CoreFoundation.h>

void InitializePlatform() {
    // recall: iOS gets weird if you call a directory "Resources" in the base of the .app,
    //   so the stuff is just copied in directly
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) {
        fprintf(stderr, "Problem setting up working directory! Probably nothing will work!\n");
    }
    CFRelease(resourcesURL);
    chdir(path);}

void FinalizePlatform() {
}
