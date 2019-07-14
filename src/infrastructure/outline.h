#pragma once

#include "world.h"

typedef struct {
    
} Outline;

Outline* CreateOutline(TileSet* ts);
void UpdateOutline(Outline* o, TileSet* ts);
void DestroyOutline(Outline* o);

void RenderOutline(Outline* o);
