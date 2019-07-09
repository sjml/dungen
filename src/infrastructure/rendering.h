#pragma once
#include "../stdafx.h"

typedef struct {
    int windowWidth;
    int windowHeight;
    double aperture;
    hmm_vec3 position;
    hmm_vec3 view;
    hmm_vec3 up;
    
    float zNearClip;
    float zFarClip;
} CameraData;

void InitializeRendering(void);
void FinalizeRendering(void);

int Render(void);
