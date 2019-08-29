%module dungen
%{
    #include "../infrastructure/rendering.h"
%}

void DumpScreenShot(const char* fileName);
gbVec2 WorldToScreen(gbVec2 worldCoordinates);
