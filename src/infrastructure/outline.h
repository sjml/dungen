#pragma once

#include "world.h"

typedef struct sOutline {
    gbVec4 color;
    GLsizei numPoints;
    GLuint vao;
    GLuint vbo;
} Outline;

Outline* CreateOutline(TileSet* ts, float thickness, int type);
void DestroyOutline(Outline* o);
