%module dungen
%{
    #include "../infrastructure/world.h"
%}


%nodefaultdtor TileData;
typedef struct {
    long long i;
    Vec2i hexPos;
    gbVec2 worldPos;
    gbVec3 color;

    int neighborW;
    int neighborNW;
    int neighborNE;
    int neighborE;
    int neighborSE;
    int neighborSW;

    Region** memberRegions;
} TileData;

%nodefaultctor sRegion;
%nodefaultdtor sRegion;
typedef struct sRegion {
    long long i;
    TileNEWSet* tiles;
} Region;

%extend sRegion {
    sRegion() {
        return CreateRegion();
    }
}

%nodefaultctor sTileNewSet;
%nodefaultctor sTileNewSet;
typedef struct sTileNEWSet {} TileNEWSet;
%extend sTileNEWSet {
    sTileNEWSet() {
        return NULL;
    }
}

void InitializeWorld(int width, int height, float scale);
Vec2i GetWorldDimensions(void);
float GetWorldScale(void);

TileData** GetAllTiles();
void SetTileAsDirty(TileData* td);
TileData** GetDirtyTiles(void);
void CleanAllTiles(void);
TileData* GetTileAtPosition(int x, int y);
TileData* GetTileAtIndex(long long i);

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
