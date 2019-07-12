%module dungen
%{
    #include "../world.h"
%}

typedef struct {
    int i;
    Vec2i hexPos;
    gbVec3 color;
    
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

TileData* GetTileAtPosition(int x, int y);
TileData* GetTileAtIndex(int i);
// GetTileNeighbors intentionally omitted in favor of Lua version
// TileData** GetTileNeighbors(TileData* center, int *numNeighbors);

