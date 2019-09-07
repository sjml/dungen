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
    TileSet* tiles;
} Region;

%extend sRegion {
    sRegion() {
        return CreateRegion();
    }
}

%nodefaultctor Agent;
%nodefaultdtor Agent;
typedef struct {
    long long i;
    Region* domain;
} Agent;

%extend Agent {
    Agent() {
        return CreateAgent();
    }
}

%nodefaultctor sTileSet;
%nodefaultctor sTileSet;
typedef struct sTileSet {} TileSet;
%extend sTileSet {
    sTileSet() {
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
TileData* ScreenToTile(gbVec2* screenCoordinates);
TileData** GetTileNeighbors(TileData* center);
TileData** GetTileCircle(TileData* center, int radius);
Agent* GetTileOwner(TileData* t);
void SetTileOutline(TileData* t, gbVec4 color, float thickness, int type);
void ClearTileOutline(TileData* t);


void DestroyTileSet(TileSet* ts);
TileSet* AddTileToSet(TileSet* ts, TileData* t);
TileSet* RemoveTileFromSet(TileSet* ts, TileData* t);
bool IsTileInSet(TileSet* ts, TileData* t);
long GetTileSetCount(TileSet* ts);
TileData** GetTilesFromSet(TileSet* ts);
TileSet* IntersectTileSets(TileSet* set1, TileSet* set2);


Region* CreateRegion(void);
void DestroyRegion(Region* r);
void SetRegionOutline(Region* r, gbVec4 color, float thickness, int type);
void ClearRegionOutline(Region* r);
gbVec2 GetRegionCenterPoint(Region* r);
void SetRegionLabel(Region* r, const char* text, float scale, gbVec4 color, gbVec2 tileOffset);
void ClearRegionLabel(Region* r);
void AddTileToRegion(Region* r, TileData* t);
void RemoveTileFromRegion(Region* r, TileData* t);
void SetRegionAsDirty(Region* r);
DisposableRegionList GetDirtyRegions(void);
void CleanAllRegions(void);
void SetRegionParent(Region* child, Region* parent);
Region* GetRegionParent(Region* r);
Region** GetRegionChildren(Region* r);


Agent* CreateAgent(void);
Agent** GetAllAgents(void);

