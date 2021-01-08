#pragma once

#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_OSX
        #define DUNGEN_MOBILE 0
    #else
        #define DUNGEN_MOBILE 1
    #endif // TARGET_OS_OSX
#endif // __APPLE__

#ifdef __EMSCRIPTEN__
    #define DUNGEN_WASM 1
#else
    #define DUNGEN_WASM 0
#endif // __EMSCRIPTEN__


void InitializeDunGen(const char* startupElement);
void TickAndRender(void);
void EventCallback(const sapp_event* event);
void FinalizeDunGen(void);
