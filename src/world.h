#pragma once

typedef struct {
    int i;
    vec3 color;
} TileData;

void InitializeWorld(int width, int height, float scale);
void RenderTiles(void);

TileData* GetTileAtPosition(int x, int y);
TileData* GetTileAtIndex(int i);

