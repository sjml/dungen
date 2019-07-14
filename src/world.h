#pragma once

#include "infrastructure/util.h"

typedef struct {
    long long i;
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

typedef struct {
    TileData* key;
    int value;
} TileHash;
typedef struct {
    TileHash* tiles;
} TileSet;

void InitializeWorld(int width, int height, float scale);
void FinalizeWorld(void);
Vec2i GetWorldDimensions(void);
float GetWorldScale(void);

void RenderTiles(void);

TileData* GetTileAtPosition(int x, int y);
TileData* GetTileAtIndex(long long i);
TileData** GetTileNeighbors(TileData* center, int *numNeighbors);

int AddTileToSet(TileSet* ts, TileData* t);
int RemoveTileFromSet(TileSet* ts, TileData* t);
bool IsTileInSet(TileSet* ts, TileData* t);
int GetTileCount(TileSet* ts);
TileData** GetTiles(TileSet* ts);
void DestroyTileSet(TileSet* ts);
