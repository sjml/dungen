#pragma once

#include "world.h"

typedef struct sOutline {
    gbVec4 color;
    float thickness;
    float** pointLists;
} Outline;

Outline* CreateOutline(TileSet* ts, float thickness, int type);
void DestroyOutline(Outline* o);

void RenderOutline(Outline* o);
