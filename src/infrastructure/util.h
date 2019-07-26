#pragma once

typedef struct {
    int x, y;
} Vec2i;

int handleGLErrors(const char* f, const int line);

float RandomRangeFloat(float min, float max);
int RandomRangeInt(int min, int max);
