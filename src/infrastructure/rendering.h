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

typedef struct {
    sg_pipeline pipe;
    sg_bindings bind;
} DrawState;

typedef struct {
    gbVec4 color;
    gbMat4 matrix;
} BasicUniforms;

void InitializeRendering(void);
void FinalizeRendering(void);

void AddRegionToRendering(Region* r);
void RemoveRegionFromRendering(Region* r);
Region** GetRenderingRegions(void);

void UpdateRenderingDimensions(void);
Vec2i GetWindowDimensions(void);
Vec2i GetFramebufferDimensions(void);
Vec2i GetOrthoDimensions(void);

gbVec2 WorldToScreen(gbVec2 worldCoordinates);
gbVec2 ScreenToOrtho(gbVec2 screenCoordinates);

void ClearTextLabels(void);
void AddTextLabel(const char* text, gbVec2 pos, float scale, gbVec4 color);

void DrawShapeBuffer(sg_buffer buff, int numPoints, gbVec4 color, gbMat4 *matrix);

void RequestRenderBufferUpdate(TileSet* ts);

int Render(void);
