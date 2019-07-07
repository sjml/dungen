#include "../stdafx.h"
#include "util.h"

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

double clampd(double d, double min, double max) {
    const double t = d < min ? min : d;
    return t > max ? max : t;
}

float clampf(float f, float min, float max) {
    const float t = f < min ? min : f;
    return t > max ? max : t;
}

int handleGLErrors(const char* f, int line) {
    bool errorFound = false;
    GLenum errorCode;
    const char* errorString;
    do
    {
        errorCode = glGetError();
        if (errorCode != GL_NO_ERROR)
        {
            errorFound = true;
            
            switch (errorCode)
            {
                case GL_INVALID_ENUM:
                    errorString = "unacceptable value for enumerated argument";
                    break;
                case GL_INVALID_VALUE:
                    errorString = "argument value out of range";
                    break;
                case GL_INVALID_OPERATION:
                    errorString = "the specified argument is not allowed in the current state";
                    break;
                case GL_STACK_OVERFLOW:
                    errorString = "internal stack overflow";
                    break;
                case GL_STACK_UNDERFLOW:
                    errorString = "internal stack underflow";
                    break;
                case GL_OUT_OF_MEMORY:
                    errorString = "not enough memory to execute command";
                    break;
                default:
                    errorString = "unknown error";
            }
            
            printf("OpenGL Error in %s (%d): %s", f, line, errorString);
        }
    } while (errorCode != GL_NO_ERROR);
    
    return errorFound;
}
