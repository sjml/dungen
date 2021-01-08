#pragma once

#include "world.h"

typedef struct sOutline {
    gbVec4 color;
    size_t numPoints;
    sg_buffer buffer;
    sg_bindings bindings;
} Outline;

Outline* CreateOutline(TileSet* ts, float thickness, int type);
void DestroyOutline(Outline* o);
