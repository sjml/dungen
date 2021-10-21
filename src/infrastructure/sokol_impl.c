#include "stdafx.h"

// Note: must be compiled as Objective-C on macOS and iOS.

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wshorten-64-to-32"
    #pragma clang diagnostic ignored "-Wstrict-prototypes"
#endif // __clang__

#define SOKOL_APP_IMPL
#include <sokol_app.h>

#define SOKOL_GFX_IMPL
#include <sokol_gfx.h>

#define SOKOL_GLUE_IMPL
#include <sokol_glue.h>

#define SOKOL_TIME_IMPL
#include <sokol_time.h>

#define SOKOL_ARGS_IMPL
#include <sokol_args.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#define SOKOL_IMGUI_IMPL
#include <util/sokol_imgui.h>

#define SOKOL_EXT_IMPL
#include "sokol_ext.h"

#ifdef __clang__
    #pragma clang diagnostic pop
#endif // __clang__
