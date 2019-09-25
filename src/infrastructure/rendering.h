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

void DumpScreenShot(const char* fileName);

void AddRegionToRendering(Region* r);
void RemoveRegionFromRendering(Region* r);
Region** GetRenderingRegions(void);

Vec2i GetWindowDimensions(void);
Vec2i GetFramebufferDimensions(void);
Vec2i GetOrthoDimensions(void);
void SetWindowDimensions(Vec2i dims);
void SetFramebufferDimensions(Vec2i dims);

gbVec2 GetCursorPosition(void);
gbVec2 WorldToScreen(gbVec2 worldCoordinates);
gbVec2 ScreenToWorld(gbVec2 screenCoordinates);
gbVec2 ScreenToOrtho(gbVec2 screenCoordinates);

void ClearTextLabels(void);
void AddTextLabel(const char* text, gbVec2 pos, float scale, gbVec4 color);

GLuint LoadProgram(const char* vertexFile, const char* fragmentFile);
void UpdateRenderBuffers(TileSet* ts);

int Render(void);
