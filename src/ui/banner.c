#include "stdafx.h"
#include "banner.h"

#include "../infrastructure/text.h"
#include "../infrastructure/rendering.h"

typedef struct {
    TextInfo ti;
    gbVec2 extents;
    gbVec4 bgColor;
    float verts[8];
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

    Vec2i dims = GetOrthoDimensions();

    float y = (dims.y * 0.5f) + (height * -0.5f);
    float x = dims.x * 0.5f;
    for (long i = 0; i < arrlen(banners); i++) {
        if (banners[i].manualY >= 0.0f) {
            banners[i].ti.pos.x = x - (banners[i].extents.x * 0.5f);
            banners[i].ti.pos.y = banners[i].manualY;
        }
        else {
            banners[i].ti.pos.x = x - (banners[i].extents.x * 0.5f);
            banners[i].ti.pos.y = y + (banners[i].extents.y);
            y += banners[i].extents.y + padding;
        }

        float x0, y0, x1, y1;
        x0 = 0.0f;
        x1 = (float)dims.x;
        y0 = banners[i].ti.pos.y + (padding * 0.5f);
        y1 = banners[i].ti.pos.y - (banners[i].extents.y + (padding * 0.5f));
        banners[i].verts[0] = x0;
        banners[i].verts[1] = y0;
        banners[i].verts[2] = x1;
        banners[i].verts[3] = y0;
        banners[i].verts[4] = x0;
        banners[i].verts[5] = y1;
        banners[i].verts[6] = x1;
        banners[i].verts[7] = y1;
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
        glColor4f(banners[i].bgColor.r, banners[i].bgColor.g, banners[i].bgColor.b, banners[i].bgColor.a);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glVertexPointer(2, GL_FLOAT, 0, banners[i].verts);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glPopMatrix();

        glColor4f(banners[i].ti.color.r, banners[i].ti.color.g, banners[i].ti.color.b, banners[i].ti.color.a);
        DrawGameText(banners[i].ti.text, "fonts/04B_03__.TTF", banners[i].ti.scale, (int)banners[i].ti.pos.x, (int)banners[i].ti.pos.y, 0.0f);
    }
}
