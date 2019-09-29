#pragma once

#include "dungen.h"

#include <stdlib.h>
#include <stdbool.h>
#include <float.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
    #define GLFW_INCLUDE_ES3
#else
	#if _WIN32
		#include <glad/glad.h>
	#else
		#define GLFW_INCLUDE_GLCOREARB
	#endif // _WIN32
#endif // __EMSCRIPTEN__

#if !(DUNGEN_MOBILE)
    #ifdef __clang__
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdocumentation"
    #endif // __clang__
    #include <GLFW/glfw3.h>
    #ifdef __clang__
        #pragma clang diagnostic pop
    #endif // __clang__
#else
    #include <OpenGLES/ES3/gl.h>
#endif // !(DUNGEN_MOBILE)

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
