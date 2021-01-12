%module dungen
%{
    #include "../infrastructure/rendering.h"
%}

gbVec2 WorldToScreen(gbVec2 worldCoordinates);
void TakeScreenshot(const char* fileName);
