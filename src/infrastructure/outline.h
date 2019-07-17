#pragma once

#include "world.h"

typedef struct {
    gbVec3 color;
    float** pointLists;
} Outline;

Outline* CreateOutline(TileSet* ts);
void DestroyOutline(Outline* o);

void RenderOutline(Outline* o);
