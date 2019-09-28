#pragma once

typedef struct {
    int x, y;
} Vec2i;

// quick and dirty; won't work on very large files; make sure to free()
char* readTextFile(const char* filename);
unsigned char* readBinaryFile(const char* filename);

int handleGLErrors(const char* f, const int line);
#define GL_CHECK() handleGLErrors(__FILE__, __LINE__)
