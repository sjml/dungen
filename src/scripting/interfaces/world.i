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

    TileSet** memberSets;
} TileData;

%nodefaultctor sTileSet;
%nodefaultdtor sTileSet;
typedef struct sTileSet {
    long long i;
} TileSet;

%extend sTileSet {
    sTileSet() {
        return CreateTileSet();
    }
}

void InitializeWorld(int width, int height, float scale);
Vec2i GetWorldDimensions(void);
float GetWorldScale(void);

TileData** GetAllTiles();
TileData* GetTileAtPosition(int x, int y);
TileData* GetTileAtIndex(long long i);

void DestroyTileSet(TileSet* ts);
int AddTileToSet(TileSet* ts, TileData* t);
int RemoveTileFromSet(TileSet* ts, TileData* t);
bool IsTileInSet(TileSet* ts, TileData* t);
int GetTileCount(TileSet* ts);
TileData** GetTiles(TileSet* ts);
void SetTileSetOutline(TileSet* ts, gbVec4 color, float thickness);
void ClearTileSetOutline(TileSet* ts);
