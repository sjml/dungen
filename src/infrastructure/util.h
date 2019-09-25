#pragma once

typedef struct {
    int x, y;
} Vec2i;

char* stringFromFile(const char* filename);
int handleGLErrors(const char* f, const int line);

