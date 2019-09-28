#pragma once

typedef struct {
    char* text;
    gbVec2 pos;
    float scale;
    gbVec4 color;
} TextInfo;

void InitializeText(void);
void FinalizeText(void);

void LoadFont(const char* refName, const char* filePath, float pointSize, bool isPixelFont);
bool PurgeFont(const char* path);

void PrepDrawText(gbMat4* matrix);
void FinishDrawText(void);
void DrawText(const char* fontName, const char* textString, gbVec2 pos, gbVec4 color, float scale);

gbVec2 MeasureTextExtents(const char* text, const char* fontName, float scale);
