#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <float.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#endif // __clang__
#include <GLFW/glfw3.h>
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
