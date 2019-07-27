#pragma once

void InitializeText(void);
void FinalizeText(void);

bool LoadFont(const char* path);
bool PurgeFont(const char* path);
float DrawGameText(const char* text, const char* fontPath, float size, int pixelX, int pixelY, float angle);

gbVec2 MeasureTextExtents(const char* text, const char* fontPath, float size);
float GetTextAscenderHeight(const char* fontPath, float size);
