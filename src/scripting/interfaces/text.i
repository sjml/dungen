%module dungen
%{
    #include "../infrastructure/text.h"
%}

gbVec2 MeasureTextExtents(const char* text, const char* fontPath, float scale);
