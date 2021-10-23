#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <float.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#if _WIN32
    #include <glad/glad.h>
#endif // _WIN32

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wstrict-prototypes"
#endif // __clang__

#ifdef _MSC_VER
    #define strdup _strdup
#endif // _MSC_VER

#ifdef __EMSCRIPTEN__
    #define SOKOL_GLES3
    extern char* strdup(const char*);
#else
    #ifdef __APPLE__
        #include "TargetConditionals.h"
        #if TARGET_OS_OSX
            #define SOKOL_GLCORE33
            // #define SOKOL_METAL
        #else
            #define SOKOL_GLES3
            // #define SOKOL_METAL
        #endif // TARGET_OS_MAC
    #else
        #define SOKOL_GLCORE33
    #endif
#endif // __EMSCRIPTEN__

#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <sokol_time.h>
#include <sokol_args.h>
#if !defined(DUNGEN_DISABLE_TOOLS)
    #include <util/sokol_imgui.h>
#endif // !defined(DUNGEN_DISABLE_TOOLS)

#ifdef __clang__
    #pragma clang diagnostic pop
#endif // __clang__


#include <gb_math.h>

#include <stb_ds.h>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif // __clang__
#include <sds.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif // __clang__
