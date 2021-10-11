#include "stdafx.h"
#include "util.h"

// this should be getting disabled anyway by the /external flags... hrm.
#ifdef _MSC_VER
    #pragma warning( disable : 4702 )
#endif // _MSC_VER

    #define STB_DS_IMPLEMENTATION
    #include <stb_ds.h>

#ifdef _MSC_VER
    #pragma warning ( default : 4702 )
#endif // _MSC_VER

#define GB_MATH_IMPLEMENTATION
#include <gb_math.h>

char* readTextFile(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buffer = (char*)malloc((length + 1) * sizeof(char));
    if (!buffer) {
        return 0;
    }
    size_t read_count = fread(buffer, sizeof(char), length, f);
    fclose(f);
    buffer[read_count * sizeof(char)] = '\0';
    return buffer;
}

unsigned char* readBinaryFile(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (f == NULL) {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* buffer = (unsigned char*)malloc(length);
    if (!buffer) {
        return 0;
    }
    /* size_t read_count = */ fread(buffer, 1, length, f);
    fclose(f);
    return buffer;
}
