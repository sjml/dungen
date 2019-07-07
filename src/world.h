#pragma once

typedef struct {
    vec3 color;
} TileData;


void InitializeWorld(int width, int height, float scale);
void RenderTiles(void);
