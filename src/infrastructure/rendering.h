#pragma once
#include "../stdafx.h"

#include "world.h"

typedef struct {
    int windowWidth;
    int windowHeight;
    float aperture;
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

gbVec2 WorldToScreen(gbVec2 worldCoordinates);
gbVec2 ScreenToWorld(gbVec2* screenCoordinates);

void ClearTextStrings(void);
void AddTextString(const char* text, gbVec2 pos, float scale, gbVec4 color);

int Render(void);
