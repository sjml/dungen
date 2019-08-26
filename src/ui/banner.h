#pragma once

void AddBanner(const char* text, float scale, gbVec4 textColor, gbVec4 bgColor, float duration);

bool UpdateBanners(float dt);
void RenderBanners(void);
