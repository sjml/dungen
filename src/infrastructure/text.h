#pragma once

void InitializeText(void);
void FinalizeText(void);

bool LoadFont(const char* path);
bool PurgeFont(const char* path);
float DrawGameText(const char* text, const char* fontPath, float size, int pixelX, int pixelY, float angle);
void PrintTextString(char *text, gbVec2* pos, gbVec4* color, float scale, float spacing);
