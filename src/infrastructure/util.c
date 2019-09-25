#include "stdafx.h"
#include "util.h"

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#define GB_MATH_IMPLEMENTATION
#include <gb_math.h>

// quick and dirty; won't work on very large files; fine for reading in shaders to compile
char* stringFromFile(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        return 0;
    }
    /* size_t read_count = */ fread(buffer, 1, length, f);
    fclose(f);
    buffer[length] = '\0';
    return buffer;
}

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
