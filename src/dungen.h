#pragma once

#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_OSX
        #define DUNGEN_MOBILE 0
    #else
        #define DUNGEN_MOBILE 1
    #endif // TARGET_OS_OSX
#endif // __APPLE__



void InitializeDunGen(const char* startupElement);
void RunDunGen(void);
void FinalizeDunGen(void);
