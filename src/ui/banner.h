#pragma once

void* AddBanner(const char* text, float scale, gbVec4 textColor, gbVec4 bgColor, float duration);
void PositionBanner(void* bannerHandle, float yPos);
void RemoveBanner(void* bannerHandle);
void RepositionBanners(void);

bool UpdateBanners(float dt);
void RenderBanners(gbMat4* matrix);
