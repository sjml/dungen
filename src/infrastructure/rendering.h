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

GLFWwindow* GetWindowHandle(void);

void AddRegionToRendering(Region* r);
void RemoveRegionFromRendering(Region* r);
Region** GetRenderingRegions(void);

gbVec2 WorldToScreen(gbVec2 worldCoordinates);
gbVec2 ScreenToWorld(gbVec2 screenCoordinates);

void ClearTileLabels(void);
void AddTileLabel(const char* text, gbVec2 pos, float scale, gbVec4 color);

int Render(void);
