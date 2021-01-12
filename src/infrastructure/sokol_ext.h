#if defined(SOKOL_IMPL) && !defined(SOKOL_EXT_IMPL)
#define SOKOL_EXT_IMPL
#endif
#ifndef SOKOL_EXT_INCLUDED
/*
    sgext_read_framebuffer -- useful for screenshots
        based on notes from https://github.com/floooh/sokol/issues/282
*/
#define SOKOL_EXT_INCLUDED (1)

#if defined(SOKOL_API_DECL) && !defined(SOKOL_EXT_API_DECL)
    #define SOKOL_EXT_API_DECL SOKOL_API_DECL
#endif
#ifndef SOKOL_EXT_API_DECL
    #if defined(_WIN32) && defined(SOKOL_DLL) && defined(SOKOL_GFX_IMPL)
        #define SOKOL_EXT_API_DECL __declspec(dllexport)
    #elif defined(_WIN32) && defined(SOKOL_DLL)
        #define SOKOL_EXT_API_DECL __declspec(dllimport)
    #else
        #define SOKOL_EXT_API_DECL extern
    #endif // defined(_WIN32) && defined(SOKOL_DLL) && defined(SOKOL_GFX_IMPL)
#endif // SOKOL_EXT_API_DECL

#ifdef __cplusplus
extern "C" {
#endif

SOKOL_EXT_API_DECL void sgext_read_framebuffer(int startx, int starty, int width, int height, unsigned char* pixels);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SOKOL_EXT_INCLUDED

/*--- IMPLEMENTATION ---------------------------------------------------------*/
#ifdef SOKOL_EXT_IMPL
#define SOKOL_EXT_IMPL_INCLUDED (1)


SOKOL_EXT_API_DECL void sgext_read_framebuffer(int startx, int starty, int width, int height, unsigned char* pixels) {
#if defined(_SOKOL_ANY_GL)
    glReadPixels(startx, startx, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
#endif // defined(_SOKOL_ANY_GL)
}

#endif // SOKOL_EXT_IMPL
