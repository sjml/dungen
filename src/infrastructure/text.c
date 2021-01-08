#include "stdafx.h"

// NB: This file provides an implementation of flexible, good-looking font-rendering.
//     It enables the use of multiple fonts at multiple sizes without needing to prebake
//     or manage them too specifically.
//     HOWEVER: it is pretty wasteful with memory. Could use a smarter packing algorithm,
//     at the minimum, but lots of other optimizations are possible. The glyph dimension
//     lookups it has to do on every draw call could be also be cached. Et cetera.
//     Only use this if you don't care too much about memory or performance.

#include "text.h"

#include "util.h"
#include "rendering.h"
#include "../platform/platform.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wcomma"
    #pragma clang diagnostic ignored "-Wunused"
#endif // __clang__
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif // __clang__


#define MAX_VERTS (4*128)
#define TEXT_ATLAS_SIZE 512
#define TEXT_ATLAS_NUM_CHARS 256

static uint32_t textProgram;
static int32_t  textOrthoLocation;
static int32_t  textColorLocation;
static uint32_t textTexID;
static uint32_t textVAO, textVBO;

static BasicUniforms bu;
static DrawState ds;

///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define UTF8_ACCEPT 0
#define UTF8_REJECT 1

static const uint8_t utf8d[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
    8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
    0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
    0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
    0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
    1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
    1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
    1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
};

uint32_t
decode_utf8(uint32_t* state, uint32_t* codep, uint32_t byte) {
    uint32_t type = utf8d[byte];

    *codep = (*state != UTF8_ACCEPT) ?
    (byte & 0x3fu) | (*codep << 6) :
    (0xff >> type) & (byte);

    *state = utf8d[256 + *state*16 + type];
    return *state;
}
// END OF Bjoern Hoehrmann COPYRIGHTED CODE
///////////////////////////////////////////////////////////////////////////////


typedef struct {
    size_t atlasIndex;
    stbtt_packedchar charData;
} GlyphData;

typedef struct {
    const char* filePath;
    float scale;
    bool isPixelFont;
    struct { uint32_t key; GlyphData* value; } *glyphs;
    sg_bindings bindings;
    int numTextures;
} FontData;

static struct { char* key; FontData *value; } *loadedFonts;

void LoadFont(const char* refName, const char* filePath, float pointSize, bool isPixelFont) {
    const unsigned char* fontBuffer = readBinaryFile(filePath);
    if (fontBuffer == NULL) {
        fprintf(stderr, "ERROR: Could not load font %s\n", filePath);
        return;
    }

    FontData* data = (FontData*)malloc(sizeof(FontData));
    data->scale = pointSize;
    data->isPixelFont = isPixelFont;
    data->glyphs = NULL;
    data->numTextures = 0;
	data->filePath = strdup(filePath);

    shput(loadedFonts, refName, data);

    struct { uint32_t key; GlyphData value; } *leftovers = NULL;
    bool donePacking = false;

    stbtt_pack_range charSet;
    charSet.first_unicode_codepoint_in_range = 0;
    charSet.array_of_unicode_codepoints = NULL;
    charSet.num_chars                   = TEXT_ATLAS_NUM_CHARS;
    charSet.font_size                   = pointSize;

    for (uint32_t i = 0; i < TEXT_ATLAS_NUM_CHARS; i++) {
        arrpush(charSet.array_of_unicode_codepoints, i);
    }

    while (!donePacking) {
        unsigned char* localAtlas = calloc(TEXT_ATLAS_SIZE * TEXT_ATLAS_SIZE, sizeof(unsigned char));
        charSet.chardata_for_range = malloc(sizeof(stbtt_packedchar) * charSet.num_chars);

        stbtt_pack_context packingContext;
        stbtt_PackBegin(&packingContext, localAtlas, TEXT_ATLAS_SIZE, TEXT_ATLAS_SIZE, 0, 1, NULL);

        for (int j=0; j < charSet.num_chars; ++j) {
            charSet.chardata_for_range[j].x0 =
            charSet.chardata_for_range[j].y0 =
            charSet.chardata_for_range[j].x1 =
            charSet.chardata_for_range[j].y1 = 0;
        }

        stbrp_rect* rects = (stbrp_rect*) malloc(sizeof(*rects) * charSet.num_chars);

        stbtt_fontinfo info;
        stbtt_InitFont(&info, fontBuffer, 0);

        int n = stbtt_PackFontRangesGatherRects(&packingContext, &info, &charSet, 1, rects);
        stbtt_PackFontRangesPackRects(&packingContext, rects, n);

        donePacking = stbtt_PackFontRangesRenderIntoRects(&packingContext, &info, &charSet, 1, rects);
        stbtt_PackEnd(&packingContext);

        sg_image_desc desc = (sg_image_desc){
            .width = TEXT_ATLAS_SIZE,
            .height = TEXT_ATLAS_SIZE,
            .content.subimage[0][0] = {
                .ptr = localAtlas,
                .size = sizeof(localAtlas)
            },
            .pixel_format = SG_PIXELFORMAT_R8,
            .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
            .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        };
        if (isPixelFont) {
            desc.min_filter = SG_FILTER_NEAREST;
            desc.mag_filter = SG_FILTER_NEAREST;
        }
        else {
            desc.min_filter = SG_FILTER_LINEAR;
            desc.mag_filter = SG_FILTER_LINEAR;
        }

        sg_image tex = sg_make_image(&desc);
        data->bindings = (sg_bindings) {
            .fs_images[0] = tex
        };
        data->numTextures += 1;

        for (int i=0; i < charSet.num_chars; i++) {
            if (rects[i].was_packed) {
                GlyphData* gd = malloc(sizeof(GlyphData));
                gd->atlasIndex = data->numTextures - 1;
                gd->charData = charSet.chardata_for_range[i];
                hmput(data->glyphs, charSet.array_of_unicode_codepoints[i], gd);
            }
        }

        if (!donePacking) {
            assert(false); // haven't implemented multi-texture font rendering
                           //    in the swap to sokol
            int* leftovers = NULL;
            for (int i=0; i < charSet.num_chars; i++) {
                if (!rects[i].was_packed) {
                    arrpush(leftovers, charSet.array_of_unicode_codepoints[i]);
                }
            }
            arrfree(charSet.array_of_unicode_codepoints);
            charSet.array_of_unicode_codepoints = leftovers;
            charSet.num_chars = (int)arrlen(charSet.array_of_unicode_codepoints);

            free(charSet.chardata_for_range);
            charSet.chardata_for_range = malloc(sizeof(stbtt_packedchar) * charSet.num_chars);
        }

        free(rects);
        free(localAtlas);
    }
    free(charSet.chardata_for_range);
    arrfree(charSet.array_of_unicode_codepoints);

    free((void*)fontBuffer);
    return;
}

void InitializeText() {
    sds vsPath = GetShaderPath("text_vs");
    sds fsPath = GetShaderPath("text_fs");
    sg_shader textShader = sg_make_shader(&(sg_shader_desc){
        .vs.source = readTextFile(vsPath),
        .fs.source = readTextFile(fsPath),
        #ifdef SOKOL_METAL
            .vs.entry = "main0",
            .fs.entry = "main0",
        #endif
        .vs.uniform_blocks[0] = {
            .size = sizeof(gbMat4),
            .uniforms = {
                [0] = { .name = "ortho", .type = SG_UNIFORMTYPE_MAT4 }
            }
        },
        .fs.images[0] = { .name="textAtlas", .type=SG_IMAGETYPE_2D },
        .fs.uniform_blocks[0] = {
            .size = sizeof(gbVec4),
            .uniforms = {
                [0] = { .name = "color_data", .type = SG_UNIFORMTYPE_FLOAT4, .array_count = 1 }
            }
        }
    });
    sdsfree(vsPath);
    sdsfree(fsPath);

    ds.pipe = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = textShader,
        .layout = {
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT4
            }
        },
        .blend = {
            .enabled = true,
            .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
            .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA
        },
        .primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP
    });
}

void FinalizeText() {
    while (shlen(loadedFonts) > 0) {
        const char* firstName = loadedFonts[0].key;
        PurgeFont(firstName);
    }
}

bool PurgeFont(const char* fontName) {
    if (shgeti(loadedFonts, fontName) < 0) {
        return false;
    }

    FontData* fd = shget(loadedFonts, fontName);
    free((void*)fd->filePath);
    hmfree(fd->glyphs);
    for (int t=0; t < fd->numTextures; t++) {
        sg_destroy_image(fd->bindings.fs_images[t]);
    }
	free(fd);

    shdel(loadedFonts, fontName);
    return true;
}

TextInfo* CreateTextInfo(const char* text, const char* fontName, gbVec2 pos, float scale, gbVec4 color) {
    FontData* fd = shget(loadedFonts, fontName);
    if (fd == NULL) {
        fprintf(stderr, "ERROR: No loaded font named %s\n", fontName);
        return NULL;
    }

    TextInfo* ti = malloc(sizeof(TextInfo));

    ti->text = malloc(sizeof(char) * strlen(text) + 1);
    strcpy(ti->text, text);
    ti->fontName = malloc(sizeof(char) * strlen(fontName) + 1);
    strcpy(ti->fontName, fontName);

    ti->scale = scale;
    ti->color = color;

    RepositionTextInfo(ti, pos);

    float x = 0;
    float y = 0;

    float* vertList = NULL;
    ti->numGlyphs = 0;
    uint32_t state = 0;
    uint32_t codepoint = 0;
    int ci = 0;
    unsigned long textLength = strlen(ti->text);
    for (int ci=0; ci < textLength; ci++) {
        char* textPtr = &ti->text[ci];
        decode_utf8(&state, &codepoint, *(unsigned char*)textPtr);
        if (state != UTF8_ACCEPT) {
            continue;
        }

        GlyphData* gd = hmget(fd->glyphs, codepoint);

        float xpos = x + (gd->charData.xoff * ti->scale);
        float ypos = y + (gd->charData.yoff * ti->scale);
        float w = (gd->charData.x1 - gd->charData.x0) * ti->scale;
        float h = (gd->charData.y1 - gd->charData.y0) * ti->scale;

        //degenerate triangle headers
        if (ci == 0) {
            arrpush(vertList, xpos);  arrpush(vertList, ypos);
            arrpush(vertList, gd->charData.x0); arrpush(vertList, gd->charData.y0);
        }
        arrpush(vertList, xpos);  arrpush(vertList, ypos);
        arrpush(vertList, gd->charData.x0); arrpush(vertList, gd->charData.y0);

        arrpush(vertList, xpos);  arrpush(vertList, ypos);
        arrpush(vertList, gd->charData.x0); arrpush(vertList, gd->charData.y0);

        arrpush(vertList, xpos);  arrpush(vertList, ypos + h);
        arrpush(vertList, gd->charData.x0); arrpush(vertList, gd->charData.y1);

        arrpush(vertList, xpos + w);  arrpush(vertList, ypos);
        arrpush(vertList, gd->charData.x1); arrpush(vertList, gd->charData.y0);

        arrpush(vertList, xpos + w);  arrpush(vertList, ypos + h);
        arrpush(vertList, gd->charData.x1); arrpush(vertList, gd->charData.y1);

        // add degenerate triangle to connect to next character
        arrpush(vertList, xpos + w);  arrpush(vertList, ypos + h);
        arrpush(vertList, gd->charData.x1); arrpush(vertList, gd->charData.y1);
        if (ci == textLength-1) {
            arrpush(vertList, xpos + w);  arrpush(vertList, ypos + h);
            arrpush(vertList, gd->charData.x1); arrpush(vertList, gd->charData.y1);
        }

        x += gd->charData.xadvance * ti->scale;
        ti->numGlyphs++;
    }
    ti->bindings = (sg_bindings) {
        .vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
            .size = sizeof(float) * (int)arrlen(vertList),
            .content = vertList,
        }),
        .fs_images[0] = fd->bindings.fs_images[0],
    };
    arrfree(vertList);

    return ti;
}

void DestroyTextInfo(TextInfo* ti) {
    free(ti->text);
    free(ti->fontName);
    sg_destroy_buffer(ti->bindings.vertex_buffers[0]);
    free(ti);
}

void RepositionTextInfo(TextInfo* ti, gbVec2 newPos) {
    ti->pos = newPos;
    gb_mat4_translate(&ti->matrix, (gbVec3){newPos.x, newPos.y, 0.0f});
}

void PrepDrawText(gbMat4* matrix) {
    sg_apply_pipeline(ds.pipe);
    bu.matrix = *matrix;
}

void FinishDrawText() {
    return;
}

void DrawText(TextInfo* ti) {
     gbMat4 posMat;
     memcpy(&posMat, &ti->matrix, sizeof(gbMat4));
     gb_mat4_mul(&posMat, &bu.matrix, &posMat);
     sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &posMat, sizeof(gbMat4));
     sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, &ti->color, sizeof(gbVec4));

     sg_apply_bindings(&ti->bindings);
     sg_draw(0, ti->numGlyphs * 6, 1);
}

gbVec2 MeasureTextExtents(const char* text, const char* fontName, float scale) {
    gbVec2 ret = {0.0f, 0.0f};

    FontData* fd = shget(loadedFonts, fontName);
    if (fd == NULL) {
        fprintf(stderr, "ERROR: No loaded font named %s\n", fontName);
        return ret;
    }

    uint32_t state = 0;
    uint32_t codepoint = 0;
    for (; *text; ++text) {
        decode_utf8(&state, &codepoint, *(unsigned char*)text);
        if (state != UTF8_ACCEPT) {
            continue;
        }

        GlyphData* gd = hmget(fd->glyphs, codepoint);

        float width = gd->charData.xadvance * scale;
        float height = fabs(gd->charData.yoff) * scale;
        ret.x += width;
        if (height > ret.y) {
            ret.y = height;
        }
    }
    return ret;
}
