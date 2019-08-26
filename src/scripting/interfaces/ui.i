%module dungen
%{
    #include "../ui/banner.h"
    #include "../ui/choice.h"
    #include "../ui/tile_choice.h"
%}

void AddBanner(const char* text, float scale, gbVec4 textColor, gbVec4 bgColor, float duration);

void AddChoice(const char* description);
void PresentChoiceSelection();
void ClearChoices(void);

void PresentTileChoice(void);
