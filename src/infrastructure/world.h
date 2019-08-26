#pragma once

#include "util.h"

typedef struct sOutline     Outline;
typedef struct sRegion      Region;
typedef struct sTileNEWSet  TileNEWSet;

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

    Region** memberRegions;
} TileData;

typedef struct sRegion {
    long long i;
    TileNEWSet* tiles;
    Outline* outline;
} Region;

typedef struct sTileNEWSet {
    TileData* key;
    int value;
} TileNEWSet;

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
Region* CreateRegion(void);
void DestroyRegion(Region* r);
void SetRegionOutline(Region* r, gbVec4 color, float thickness);
void ClearRegionOutline(Region* r);
void AddTileToRegion(Region* r, TileData* t);
void RemoveTileFromRegion(Region* r, TileData* t);

void DestroyTileSet(TileNEWSet* ts);
TileNEWSet* AddTileToSet(TileNEWSet* ts, TileData* t);
TileNEWSet* RemoveTileFromSet(TileNEWSet* ts, TileData* t);
bool IsTileInSet(TileNEWSet* ts, TileData* t);
long GetTileSetCount(TileNEWSet* ts);
TileData** GetTilesFromSet(TileNEWSet* ts);
TileNEWSet* IntersectTileSets(TileNEWSet* set1, TileNEWSet* set2);

