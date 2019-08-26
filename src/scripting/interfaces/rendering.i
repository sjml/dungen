%module dungen
%{
    #include "../infrastructure/rendering.h"
%}

Region** GetRenderingRegions(void);

gbVec2 WorldToScreen(gbVec2 worldCoordinates);
void ClearTileLabels(void);
void AddTileLabel(const char* text, gbVec2 pos, float scale, gbVec4 color);
