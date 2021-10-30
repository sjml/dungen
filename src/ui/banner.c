#include "stdafx.h"
#include "banner.h"

#include "../infrastructure/text.h"
#include "../infrastructure/rendering.h"

typedef struct {
    TextInfo* ti;
    gbVec2 extents;
    gbVec4 bgColor;
    sg_buffer vertBuffer;
    float manualY;
    float duration;
} BannerInfo;

static BannerInfo* banners = NULL;
static const float padding = 60.0f;

void RepositionBanners() {
    float height = 0.0f;
    for (long i = 0; i < arrlen(banners); i++) {
        height += banners[i].extents.y + padding;
    }

    Vec2i dims = GetOrthoDimensions();

    float y = (dims.y * 0.5f) + (height * -0.5f);
    float x = dims.x * 0.5f;
    for (long i = 0; i < arrlen(banners); i++) {
        gbVec2 newPos;
        if (banners[i].manualY >= 0.0f) {
            newPos.x = x - (banners[i].extents.x * 0.5f);
            newPos.y = banners[i].manualY;
        }
        else {
            newPos.x = x - (banners[i].extents.x * 0.5f);
            newPos.y = y + (banners[i].extents.y);
            y += banners[i].extents.y + padding;
        }
        RepositionTextInfo(banners[i].ti, newPos);

        float x0, y0, x1, y1;
        x0 = 0.0f;
        x1 = (float)dims.x;
        y0 = banners[i].ti->pos.y + (padding * 0.5f);
        y1 = banners[i].ti->pos.y - (banners[i].extents.y + (padding * 0.5f));
        float verts[8];
        verts[0] = x0;
        verts[1] = y0;
        verts[2] = x1;
        verts[3] = y0;
        verts[4] = x0;
        verts[5] = y1;
        verts[6] = x1;
        verts[7] = y1;
        banners[i].vertBuffer = sg_make_buffer(&(sg_buffer_desc){
            .data = SG_RANGE(verts),
        });
    }
}

void PositionBanner(void* bannerHandle, float yPos) {
    ((BannerInfo*)bannerHandle)->manualY = yPos;
    RepositionBanners();
}

void* AddBanner(const char* text, float scale, gbVec4 textColor, gbVec4 bgColor, float duration) {
    BannerInfo bi;
    bi.ti = CreateTextInfo(text, "Pixel", (gbVec2){0.0f, 0.0f}, scale, textColor);
    bi.bgColor.r = bgColor.r;
    bi.bgColor.g = bgColor.g;
    bi.bgColor.b = bgColor.b;
    bi.bgColor.a = bgColor.a;
    bi.duration = duration;
    bi.manualY = -1.0f;

    bi.extents = MeasureTextExtents(bi.ti->text, "Pixel", bi.ti->scale);

    arrpush(banners, bi);
    RepositionBanners();

    return (void*)&banners[arrlen(banners) - 1];
}

void RemoveBanner(void* bannerHandle) {
    for (long i=0; i < arrlen(banners); i++) {
        if (&banners[i] == (BannerInfo*)bannerHandle) {
            DestroyTextInfo(banners[i].ti);
            sg_destroy_buffer(banners[i].vertBuffer);
            arrdel(banners, i);
            break;
        }
        RepositionBanners();
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
        for (ptrdiff_t i=arrlen(deadIndices)-1; i >=0; i--) {
            DestroyTextInfo(banners[deadIndices[i]].ti);
            sg_destroy_buffer(banners[deadIndices[i]].vertBuffer);
            arrdel(banners, deadIndices[i]);
        }
        RepositionBanners();
    }

    return true;
}

void RenderBanners(gbMat4* matrix) {
    if (arrlen(banners) == 0) {
        return;
    }

    for (long i=0; i < arrlen(banners); i++) {
        DrawShapeBuffer(banners[i].vertBuffer, 4, banners[i].bgColor, matrix);
    }

     PrepDrawText(matrix);
     for (long i=0; i < arrlen(banners); i++) {
         DrawText(banners[i].ti);
     }
     FinishDrawText();
}
