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

// modified from gb_random*
float RandomRangeFloat(float min, float max) {
    int int_result = RandomRangeInt(0, 2147483646); /* Prevent integer overflow */
    float result = int_result/(float)2147483646;
    result *= max - min;
    result += min;
    return result;
}

int RandomRangeInt(int min, int max) {
    unsigned int random_value = rand();
    unsigned int diff, result;
    random_value = random_value * 2147001325 + 715136305; /* BCPL generator */
    diff = max - min + 1;
    result = random_value % diff;
    result += min;
    
    return result;
}
