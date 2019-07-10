%module dungen
%{
    #include "../world.h"
%}

typedef struct {
    int i;
    gbVec3 color;
} TileData;

void InitializeWorld(int width, int height, float scale);

TileData* GetTileAtPosition(int x, int y);
TileData* GetTileAtIndex(int i);

