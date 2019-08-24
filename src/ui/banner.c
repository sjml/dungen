#include "../stdafx.h"
#include "banner.h"

#include "../infrastructure/text.h"


typedef struct {
    TextInfo ti;
    gbVec2 extents;
    gbVec4 bgColor;
    float duration;
} BannerInfo;

BannerInfo* banners = NULL;
const float padding = 20.0f;

void _repositionBanners() {
    float height = 0.0f;
    for (long i = 0; i < arrlen(banners); i++) {
        height += banners[i].extents.y + padding;
    }

    float y = (768.0f * 0.5f) + (height * -0.5f);
    float x = 1024.0f * 0.5f;
    for (long i = 0; i < arrlen(banners); i++) {
        banners[i].ti.pos.x = x - (banners[i].extents.x * 0.5f);
        banners[i].ti.pos.y = y + (banners[i].extents.y);
        y += banners[i].extents.y + padding;
    }
}

void AddBanner(const char* text, float scale, gbVec4 textColor, gbVec4 bgColor, float duration) {
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

    bi.extents = MeasureTextExtents(bi.ti.text, "fonts/04B_03__.TTF", bi.ti.scale);

    arrpush(banners, bi);
    _repositionBanners();
}

void UpdateBanners(float dt) {
    long* deadIndices = NULL;
    for (long i=0; i < arrlen(banners); i++) {
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
        DrawGameText(banners[i].ti.text, "fonts/04B_03__.TTF", banners[i].ti.scale, banners[i].ti.pos.x, banners[i].ti.pos.y, 0.0f);
    }
}
