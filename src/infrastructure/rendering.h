#pragma once
#include "../stdafx.h"

typedef struct {
    int windowWidth;
    int windowHeight;
    double aperture;
    vec3 position;
    vec3 view;
    vec3 up;
    
    float zNearClip;
    float zFarClip;
} CameraData;

void InitializeRendering(void);
void FinalizeRendering(void);

int Render(void);
