%module dungen
%{
    #include "../infrastructure/rendering.h"
%}

TileSet** GetRenderingTileSets(void);

gbVec2 WorldToScreen(gbVec2* worldCoordinates);
void ClearTextStrings(void);
void AddTextString(char* text, gbVec2* pos, float scale, gbVec4* color);
