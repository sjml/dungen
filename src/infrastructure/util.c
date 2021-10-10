#include "stdafx.h"
#include "util.h"

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

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
