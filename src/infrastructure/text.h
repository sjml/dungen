#pragma once

typedef struct {
    char* text;
    char* fontName;
    gbVec2 pos;
    float scale;
    gbVec4 color;
    gbMat4 matrix;
    sg_bindings bindings;
    int numGlyphs;
} TextInfo;

void InitializeText(void);
void FinalizeText(void);

void LoadFont(const char* refName, const char* filePath, float pointSize, bool isPixelFont);
bool PurgeFont(const char* fontName);

void PrepDrawText(gbMat4* matrix);
void FinishDrawText(void);
void DrawText(TextInfo* ti);

TextInfo* CreateTextInfo(const char* text, const char* fontName, gbVec2 pos, float scale, gbVec4 color);
void DestroyTextInfo(TextInfo* ti);
void RepositionTextInfo(TextInfo* ti, gbVec2 newPos);

gbVec2 MeasureTextExtents(const char* text, const char* fontName, float scale);
