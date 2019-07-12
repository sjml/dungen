#pragma once

#include "infrastructure/util.h"

typedef struct {
    int i;
    Vec2i hexPos;
    gbVec3 color;

    // clockwise neighbors
    int neighborW;
    int neighborNW;
    int neighborNE;
    int neighborE;
    int neighborSE;
    int neighborSW;
} TileData;

void InitializeWorld(int width, int height, float scale);
Vec2i GetWorldDimensions(void);
float GetWorldScale(void);

void RenderTiles(void);

TileData* GetTileAtPosition(int x, int y);
TileData* GetTileAtIndex(int i);
TileData** GetTileNeighbors(TileData* center, int *numNeighbors);
