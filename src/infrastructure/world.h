#pragma once

#include "util.h"

typedef struct sOutline Outline;
typedef struct sTileSet TileSet;

typedef struct {
    long long i;
    Vec2i hexPos;
    gbVec2 worldPos;
    gbVec3 color;
    gbVec4 overlayColor; // not exposed to Lua

    // clockwise neighbors
    int neighborW;
    int neighborNW;
    int neighborNE;
    int neighborE;
    int neighborSE;
    int neighborSW;

    TileSet** memberSets;
} TileData;

typedef struct {
    TileData* key;
    int value;
} TileHash;

typedef struct sTileSet {
    long long i;
    TileHash* tiles;
    Outline* outline;
} TileSet;

void InitializeWorld(int width, int height, float scale);
void FinalizeWorld(void);
Vec2i GetWorldDimensions(void);
float GetWorldScale(void);
gbVec2** GetWorldPointList(void);

void RenderTiles(void);

TileData** GetAllTiles(void);
void SetTileAsDirty(TileData* td);
TileData** GetDirtyTiles(void);
void CleanAllTiles(void);
TileData* GetTileAtPosition(int x, int y);
TileData* GetTileAtIndex(long long i);
TileData* ScreenToTile(gbVec2* screenCoordinates);
TileData** GetTileNeighbors(TileData* center, int *numNeighbors);
long GetTileDistance(TileData* t1, TileData* t2);

// TODO: TileSets will leak once they have no members if they
//       aren't tracked somehow.
//       Don't want to automatically destroy when members are 0
//       because might still be working with it. Also hard to track
//       since they could be sitting in registers or in the Lua runtime.
//       Smart pointers would help, but too heavyweight.
//       For now, I don't care about the leaks.
TileSet* CreateTileSet(void);
void DestroyTileSet(TileSet* ts);
int AddTileToSet(TileSet* ts, TileData* t);
int RemoveTileFromSet(TileSet* ts, TileData* t);
bool IsTileInSet(TileSet* ts, TileData* t);
int GetTileCount(TileSet* ts);
TileData** GetTiles(TileSet* ts);
void SetTileSetOutline(TileSet* ts, gbVec4 color, float thickness);
void ClearTileSetOutline(TileSet* ts);
