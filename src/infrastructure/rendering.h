#pragma once
#include "../stdafx.h"

#include "world.h"

typedef struct {
    int windowWidth;
    int windowHeight;
    double aperture;
    gbVec3 position;
    gbVec3 view;
    gbVec3 up;
    
    float zNearClip;
    float zFarClip;
} CameraData;

void InitializeRendering(void);
void FinalizeRendering(void);

void AddTileSetToRendering(TileSet* ts);
void RemoveTileSetFromRendering(TileSet* ts);
TileSet** GetRenderingTileSets(void);

int Render(void);
