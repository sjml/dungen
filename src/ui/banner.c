#include "stdafx.h"
#include "banner.h"

#include "../infrastructure/text.h"


typedef struct {
    TextInfo ti;
    gbVec2 extents;
    gbVec4 bgColor;
    float manualY;
    float duration;
} BannerInfo;

static BannerInfo* banners = NULL;
static const float padding = 60.0f;

void _repositionBanners() {
    float height = 0.0f;
    for (long i = 0; i < arrlen(banners); i++) {
        height += banners[i].extents.y + padding;
    }

    float y = (768.0f * 0.5f) + (height * -0.5f);
    float x = 1024.0f * 0.5f;
    for (long i = 0; i < arrlen(banners); i++) {
        if (banners[i].manualY >= 0.0f) {
            banners[i].ti.pos.x = x - (banners[i].extents.x * 0.5f);
            banners[i].ti.pos.y = banners[i].manualY;
            continue;
        }
        banners[i].ti.pos.x = x - (banners[i].extents.x * 0.5f);
        banners[i].ti.pos.y = y + (banners[i].extents.y);
        y += banners[i].extents.y + padding;
    }
}

void PositionBanner(void* bannerHandle, float yPos) {
    ((BannerInfo*)bannerHandle)->manualY = yPos;
    _repositionBanners();
}

void* AddBanner(const char* text, float scale, gbVec4 textColor, gbVec4 bgColor, float duration) {
    BannerInfo bi;
    bi.ti.text = malloc(sizeof(char) * (strlen(text) + 1));
    strcpy(bi.ti.text, text);
    bi.ti.scale = scale;
    bi.ti.color.r = textColor.r;
    bi.ti.color.g = textColor.g;
    bi.ti.color.b = textColor.b;
    bi.ti.color.a = textColor.a;
    bi.bgColor.r = bgColor.r;
    bi.bgColor.g = bgColor.g;
    bi.bgColor.b = bgColor.b;
    bi.bgColor.a = bgColor.a;
    bi.duration = duration;
    bi.manualY = -1.0f;

    bi.extents = MeasureTextExtents(bi.ti.text, "fonts/04B_03__.TTF", bi.ti.scale);

    arrpush(banners, bi);
    _repositionBanners();
    
    return (void*)&banners[arrlen(banners) - 1];
}

void RemoveBanner(void* bannerHandle) {
    for (long i=0; i < arrlen(banners); i++) {
        if (&banners[i] == (BannerInfo*)bannerHandle) {
            arrdel(banners, i);
            break;
        }
        _repositionBanners();
    }
}

bool UpdateBanners(float dt) {
    if (arrlen(banners) == 0) {
        return false;
    }

    long* deadIndices = NULL;
    for (long i=0; i < arrlen(banners); i++) {
        if (banners[i].duration < 0.0f) {
            continue; // this one is getting handled manually
        }
        banners[i].duration -= dt;
        if (banners[i].duration <= 0.0f) {
            arrpush(deadIndices, i);
        }
    }
    if (arrlen(deadIndices) > 0) {
        for (long i=arrlen(deadIndices)-1; i >=0; i--) {
            arrdel(banners, deadIndices[i]);
        }
        _repositionBanners();
    }

    return true;
}

void RenderBanners() {
    for (long i=0; i < arrlen(banners); i++) {
        glColor4fv(banners[i].bgColor.e);
        float x1 = 0.0f;
        float x2 = 1024.0f;
        float y1 = 768.0f - (banners[i].ti.pos.y + (padding * 0.5f));
        float y2 = 768.0f - (banners[i].ti.pos.y - (banners[i].extents.y + (padding * 0.5f)));

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glBegin(GL_TRIANGLE_STRIP);
            glVertex2f(x2, y1);
            glVertex2f(x2, y2);
            glVertex2f(x1, y1);
            glVertex2f(x1, y2);
        glEnd();
        glPopMatrix();

        glColor4fv(banners[i].ti.color.e);
        DrawGameText(banners[i].ti.text, "fonts/04B_03__.TTF", banners[i].ti.scale, (int)banners[i].ti.pos.x, (int)banners[i].ti.pos.y, 0.0f);
    }
}
