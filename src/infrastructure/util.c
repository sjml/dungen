#include "stdafx.h"
#include "util.h"

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#define GB_MATH_IMPLEMENTATION
#include <gb_math.h>

int handleGLErrors(const char* f, const int line) {
    bool errorFound = false;
    GLenum errorCode;
    const char* errorString;
    do
    {
        errorCode = glGetError();
        if (errorCode != GL_NO_ERROR) {
            errorFound = true;

            switch (errorCode) {
                case GL_INVALID_ENUM:
                    errorString = "GL_INVALID_ENUM";
                    break;
                case GL_INVALID_VALUE:
                    errorString = "GL_INVALID_VALUE";
                    break;
                case GL_INVALID_OPERATION:
                    errorString = "GL_INVALID_OPERATION";
                    break;
                case GL_STACK_OVERFLOW:
                    errorString = "GL_STACK_OVERFLOW";
                    break;
                case GL_STACK_UNDERFLOW:
                    errorString = "GL_STACK_UNDERFLOW";
                    break;
                case GL_OUT_OF_MEMORY:
                    errorString = "GL_OUT_OF_MEMORY";
                    break;
                default:
                    errorString = "unknown error code";
            }

            printf("OpenGL Error in %s (%d): %s\n", f, line, errorString);
        }
    } while (errorCode != GL_NO_ERROR);

    return errorFound;
}
