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

static GLuint textProgram;
static GLint  textOrthoLocation;
static GLint  textColorLocation;
static GLuint textTexID;
static GLuint textVAO, textVBO;


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
    GLuint* textAtlasIDs;
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
    data->textAtlasIDs = NULL;

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
        GLubyte* localAtlas = calloc(TEXT_ATLAS_SIZE * TEXT_ATLAS_SIZE, sizeof(GLubyte));
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

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        #if DUNGEN_MOBILE || DUNGEN_WASM
            GLenum textureFormat = GL_ALPHA;
        #else
            GLenum textureFormat = GL_RED;
        #endif
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            textureFormat,
            TEXT_ATLAS_SIZE,
            TEXT_ATLAS_SIZE,
            0,
            textureFormat,
            GL_UNSIGNED_BYTE,
            localAtlas
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        if (isPixelFont) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        arrpush(data->textAtlasIDs, textureID);
        int texIdx = (int)arrlen(data->textAtlasIDs) - 1;
        glBindTexture(GL_TEXTURE_2D, 0);

        for (int i=0; i < charSet.num_chars; i++) {
            if (rects[i].was_packed) {
                GlyphData* gd = malloc(sizeof(GlyphData));
                gd->atlasIndex = texIdx;
                gd->charData = charSet.chardata_for_range[i];
                hmput(data->glyphs, charSet.array_of_unicode_codepoints[i], gd);
            }
        }

        if (!donePacking) {
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
    textProgram = LoadProgram("text.vert", "text.frag");
    textOrthoLocation = glGetUniformLocation(textProgram, "ortho");
    textColorLocation = glGetUniformLocation(textProgram, "textColor");

    glGenVertexArrays(1, &textVAO);
    glBindVertexArray(textVAO);

    glGenBuffers(1, &textVBO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, 0, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
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
    for (int t=0; t < arrlen(fd->textAtlasIDs); t++) {
        glDeleteTextures(1, &fd->textAtlasIDs[t]);
    }
    arrfree(fd->textAtlasIDs);

    shdel(loadedFonts, fontName);
    return true;
}

void PrepDrawText(gbMat4* matrix) {
    glUseProgram(textProgram);
    glUniformMatrix4fv(textOrthoLocation, 1, GL_FALSE, (*matrix).e);

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glEnableVertexAttribArray(0);
}

void FinishDrawText() {
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void DrawText(const char* fontName, const char* textString, gbVec2 pos, gbVec4 color, float scale) {
    FontData* fd = shget(loadedFonts, fontName);
    if (fd == NULL) {
        fprintf(stderr, "ERROR: No loaded font named %s\n", fontName);
        return;
    }

    glUniform4fv(textColorLocation, 1, color.e);

    GLfloat x = pos.x;
    GLfloat y = pos.y;

    uint32_t state = 0;
    uint32_t codepoint = 0;
    for (; *textString; ++textString) {
        decode_utf8(&state, &codepoint, *(unsigned char*)textString);
        if (state != UTF8_ACCEPT) {
            continue;
        }

        GlyphData* gd = hmget(fd->glyphs, codepoint);

        GLfloat xpos = x + (gd->charData.xoff * scale);
        GLfloat ypos = y + (gd->charData.yoff * scale);
        GLfloat w = (gd->charData.x1 - gd->charData.x0) * scale;
        GLfloat h = (gd->charData.y1 - gd->charData.y0) * scale;

        GLfloat squareVerts[4 * 4] = {
            xpos    , ypos    , gd->charData.x0, gd->charData.y0,
            xpos    , ypos + h, gd->charData.x0, gd->charData.y1,
            xpos + w, ypos    , gd->charData.x1, gd->charData.y0,
            xpos + w, ypos + h, gd->charData.x1, gd->charData.y1,
        };

        glBindTexture(GL_TEXTURE_2D, fd->textAtlasIDs[gd->atlasIndex]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(squareVerts), squareVerts);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        x += gd->charData.xadvance * scale;
    }
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

        ret.x += gd->charData.xadvance * scale;
        float height = (gd->charData.y1 - gd->charData.y0) * scale;
        if (height > ret.y) {
            ret.y = height;
        }
    }
    return ret;
}
