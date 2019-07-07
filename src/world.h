#pragma once

#define worldWidth 61
#define worldHeight 53
#define tileScale 0.25f

typedef struct {
    vec3 color;
} TileData;


void InitializeWorld(void);
void RenderTiles(void);
