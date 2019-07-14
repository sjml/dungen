#pragma once
#include "../stdafx.h"

#include "outline.h"

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

void AddOutline(Outline* o);
void RemoveOutline(Outline* o);

int Render(void);
