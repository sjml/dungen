%module dungen
%{
    #include "../ui/banner.h"
%}

void AddBanner(const char* text, float scale, gbVec4 textColor, gbVec4 bgColor, float duration);
