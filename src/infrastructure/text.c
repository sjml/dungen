#include "../stdafx.h"
#include "text.h"

#include "util.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#include <ft2build.h>
#include FT_FREETYPE_H
#pragma clang diagnostic pop

#define MAX_VERTS (4*128)
#define TEXTURE_SIZE 512

FT_Library ftLib;
GLubyte* emptyTextureData = NULL;

typedef struct {
    short x, y, h;
} FontTextureRow;

typedef struct {
    GLuint id;
    float vertices[MAX_VERTS];
    int numVertices;
    FontTextureRow* rows;
} FontTexture;

typedef struct {
    FontTexture* fontTexture;
    Vec2i topLeft;
    Vec2i bottomRight;
    gbVec2 offset;
    float advance;
} Glyph;

typedef struct {
    bool isValid;
    FT_Face* ftFace;
    bool hasKerning;
    bool isDrawing;

    Vec2i textureDimensions;
    gbVec2 inverseTextureDimensions;
    FontTexture** fTextures;
    struct { unsigned int key; Glyph value; }* glyphMap;
} FontCacheEntry;

struct { const char* key; FontCacheEntry* value; }* fontCache;

FontCacheEntry* _GetFontInfo(const char* fontPath) {
    if (shgeti(fontCache, fontPath) < 0) {
        FontCacheEntry* fce = calloc(1, sizeof(FontCacheEntry));
        fce->ftFace = malloc(sizeof(FT_Face));
        int error = FT_New_Face(ftLib, fontPath, 0, fce->ftFace);
        if (error) {
            free(fce->ftFace);
            fce->isValid = false;
        }
        else {
            // TODO: implement kerning
            fce->hasKerning = (FT_HAS_KERNING(*(fce->ftFace)) != 0);

            fce->textureDimensions.x = fce->textureDimensions.y = TEXTURE_SIZE;
            fce->inverseTextureDimensions.x = 1.0f / (float)fce->textureDimensions.x;
            fce->inverseTextureDimensions.y = 1.0f / (float)fce->textureDimensions.y;

            fce->isValid = true;
        }

        fce->isDrawing = false;
        shput(fontCache, fontPath, fce);
        return fce;
    }
    else {
        return shget(fontCache, fontPath);
    }
}

void _FlushDrawing(FontCacheEntry* fce) {
    if (!fce->isValid) {
        return;
    }

    for (int i = 0; i < arrlen(fce->fTextures); i++) {
        if (fce->fTextures[i]->numVertices > 0) {
            glBindTexture(GL_TEXTURE_2D, fce->fTextures[i]->id);
            glVertexPointer(2, GL_FLOAT, sizeof(float)*4, &fce->fTextures[i]->vertices[0]);
            glTexCoordPointer(2, GL_FLOAT, sizeof(float)*4, &fce->fTextures[i]->vertices[2]);
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)fce->fTextures[i]->numVertices / 4);
            fce->fTextures[i]->numVertices = 0;
        }
    }
}

void _StartDrawing(FontCacheEntry* fce) {
    if (!fce->isValid) {
        return;
    }

    if (fce->isDrawing) {
        _FlushDrawing(fce);
    }

    fce->isDrawing = true;
}

void _EndDrawing(FontCacheEntry* fce) {
    if (!fce->isValid) {
        return;
    }

    if (!fce->isDrawing) {
        return;
    }

    _FlushDrawing(fce);
    fce->isDrawing = false;
}

Glyph _GetGlyph(FontCacheEntry* fce, float fontSize, unsigned int codepoint) {
    Glyph ret;
    ret.fontTexture = NULL;
    ret.topLeft.x = 0;
    ret.topLeft.y = 0;
    ret.bottomRight.x = 0;
    ret.bottomRight.y = 0;
    ret.advance = 0.0f;
    ret.offset.x = 0.0f;
    ret.offset.y = 0.0f;

    if (hmgeti(fce->glyphMap, codepoint) < 0) {
        // create and add glyph
        int error = 0;
        error = FT_Set_Char_Size(*fce->ftFace, 0L, fontSize * 64, 72, 72);
        error = FT_Load_Glyph(*fce->ftFace, codepoint, FT_LOAD_NO_HINTING);
        error = FT_Render_Glyph((*fce->ftFace)->glyph, FT_RENDER_MODE_NORMAL);

        int glyphWidth  = (*fce->ftFace)->glyph->bitmap.width;
        int glyphHeight = (*fce->ftFace)->glyph->bitmap.rows;
        if (glyphWidth >= fce->textureDimensions.x || glyphHeight >= fce->textureDimensions.y) {
            fprintf(stderr, "ERROR: Glyph too large for max texture size.\n");
            return ret;
        }

        FontTexture* fontTex = NULL;
        FontTextureRow* row = NULL;
        int rowHeight = (glyphHeight+7) & ~7;

        for (int texIndex = 0; texIndex < arrlen(fce->fTextures); texIndex++) {
            fontTex = fce->fTextures[texIndex];
            for (int rowIndex = 0; rowIndex < arrlen(fontTex->rows); rowIndex++) {
                // can we fit in this row?
                if (   rowHeight <= fontTex->rows[rowIndex].h
                    && fontTex->rows[rowIndex].x + glyphWidth + 1 < fce->textureDimensions.x) {
                    row = &fontTex->rows[rowIndex];
                    break;
                }
            }

            if (row == NULL) {
                // haven't found a good row in this texture; do we have room for a new one?
                int filledY = fontTex->rows[arrlen(fontTex->rows)-1].y + fontTex->rows[arrlen(fontTex->rows)-1].h;
                if (filledY + rowHeight < fce->textureDimensions.y) {
                    // put a row in
                    FontTextureRow newRow;
                    newRow.x = 0;
                    newRow.y = filledY;
                    newRow.h = rowHeight;
                    arrpush(fontTex->rows, newRow);
                    row = &fontTex->rows[arrlen(fontTex->rows)-1];
                }
            }
            // still no row; maybe in the next texture
        }

        if (row == NULL) {
            FontTexture* tex = calloc(1, sizeof(FontTexture));
            glGenTextures(1, &tex->id);
            glBindTexture(GL_TEXTURE_2D, tex->id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, fce->textureDimensions.x, fce->textureDimensions.y, 0, GL_ALPHA, GL_UNSIGNED_BYTE, emptyTextureData);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            arrpush(fce->fTextures, tex);

            fontTex = fce->fTextures[arrlen(fce->fTextures)-1];

            // put a row in
            FontTextureRow newRow;
            newRow.x = 0;
            newRow.y = 0;
            newRow.h = rowHeight;
            arrpush(fontTex->rows, newRow);
            row = &fontTex->rows[arrlen(fontTex->rows)-1];
        }

        ret.fontTexture = fontTex;
        ret.topLeft.x = row->x;
        ret.topLeft.y = row->y;
        ret.bottomRight.x = row->x + glyphWidth;
        ret.bottomRight.y = row->y + glyphHeight;
        ret.advance = (*fce->ftFace)->glyph->advance.x / 64;
        ret.offset.x = (*fce->ftFace)->glyph->bitmap_left;
        ret.offset.y = (*fce->ftFace)->glyph->bitmap_top;

        row->x += glyphWidth + 1;

        if (glyphWidth && glyphHeight) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glBindTexture(GL_TEXTURE_2D, fontTex->id);
            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                ret.topLeft.x,
                ret.topLeft.y,
                glyphWidth,
                glyphHeight,
                GL_ALPHA,
                GL_UNSIGNED_BYTE,
                (*fce->ftFace)->glyph->bitmap.buffer
            );
        }

        hmput(fce->glyphMap, codepoint, ret);
        return ret;
    }
    else {
        return hmget(fce->glyphMap, codepoint);
    }
}

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

float _DrawText(FontCacheEntry* fce, float fontSize, const char* text) {
    float dx = 0.0f;
    uint32_t state=0;
    uint32_t codepoint = 0;
    for (; *text; ++text) {
        decode_utf8(&state, &codepoint, *(unsigned char*)text);
        if (state != UTF8_ACCEPT) {
            continue;
        }

        codepoint = FT_Get_Char_Index(*fce->ftFace, codepoint);

        Glyph g = _GetGlyph(fce, fontSize, codepoint);
        FontTexture *t = g.fontTexture;

        if (t->numVertices + 24 >= MAX_VERTS) {
            _FlushDrawing(fce);
        }

        int rx, ry;
        rx = floorf(dx + g.offset.x);
        ry = floorf(g.offset.y);

        float x0, y0, x1, y1, s0, t0, s1, t1;
        x0 = x1 = rx;
        y0 = y1 = ry;
        x1 += (g.bottomRight.x - g.topLeft.x);
        y1 -= (g.bottomRight.y - g.topLeft.y);

        s0 = g.topLeft.x * fce->inverseTextureDimensions.x;
        t0 = g.topLeft.y * fce->inverseTextureDimensions.y;
        s1 = g.bottomRight.x * fce->inverseTextureDimensions.x;
        t1 = g.bottomRight.y * fce->inverseTextureDimensions.y;

        dx += g.advance;

        t->vertices[t->numVertices++] = x0;
        t->vertices[t->numVertices++] = y0;
        t->vertices[t->numVertices++] = s0;
        t->vertices[t->numVertices++] = t0;

        t->vertices[t->numVertices++] = x0;
        t->vertices[t->numVertices++] = y1;
        t->vertices[t->numVertices++] = s0;
        t->vertices[t->numVertices++] = t1;

        t->vertices[t->numVertices++] = x1;
        t->vertices[t->numVertices++] = y0;
        t->vertices[t->numVertices++] = s1;
        t->vertices[t->numVertices++] = t0;

        t->vertices[t->numVertices++] = x1;
        t->vertices[t->numVertices++] = y0;
        t->vertices[t->numVertices++] = s1;
        t->vertices[t->numVertices++] = t0;

        t->vertices[t->numVertices++] = x0;
        t->vertices[t->numVertices++] = y1;
        t->vertices[t->numVertices++] = s0;
        t->vertices[t->numVertices++] = t1;

        t->vertices[t->numVertices++] = x1;
        t->vertices[t->numVertices++] = y1;
        t->vertices[t->numVertices++] = s1;
        t->vertices[t->numVertices++] = t1;
    }

    return dx;
}


void InitializeText() {
    FT_Init_FreeType(&ftLib);
    if (ftLib == NULL) {
        fprintf(stderr, "ERROR: Could not initialize FreeType.\n");
        return;
    }
    emptyTextureData = (GLubyte*)calloc(TEXTURE_SIZE * TEXTURE_SIZE, sizeof(GLubyte));
}

void FinalizeText() {
    for (int i = 0; i < shlen(fontCache); i++) {
        for (int t = 0; t < arrlen(fontCache[i].value->fTextures); t++) {
            glDeleteTextures(1, &fontCache[i].value->fTextures[t]->id);
            arrfree(fontCache[i].value->fTextures[t]->rows);
            free(fontCache[i].value->fTextures[t]);
        }
        arrfree(fontCache[i].value->fTextures);
        FT_Done_Face(*fontCache[i].value->ftFace);
        free(fontCache[i].value->ftFace);
        free(fontCache[i].value);
    }

    free(emptyTextureData);
    FT_Done_FreeType(ftLib);
}

bool LoadFont(const char* path) {
    FontCacheEntry* fce = _GetFontInfo(path);
    return fce->isValid;
}

bool PurgeFont(const char* path) {
    if (shgeti(fontCache, path) < 0) {
        return false;
    }

    FontCacheEntry* fce = _GetFontInfo(path);
    for (int t = 0; t < arrlen(fce->fTextures); t++) {
        glDeleteTextures(1, &fce->fTextures[t]->id);
        arrfree(fce->fTextures[t]->rows);
        free(fce->fTextures[t]);
    }
    arrfree(fce->fTextures);
    FT_Done_Face(*fce->ftFace);
    free(fce->ftFace);
    free(fce);
    
    hmdel(fontCache, path);
    return true;
}

float DrawGameText(const char* text, const char* fontPath, float size, int pixelX, int pixelY, float angle) {
    FontCacheEntry* fce = _GetFontInfo(fontPath);
    if (!fce->isValid) {
        fprintf(stderr, "ERROR: Could not load font `%s`.\n", fontPath);
        return 0.0f;
    }

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, 1024.0f, 0.0f, 768.0f, -1, 1);
    handleGLErrors(__FILE__, __LINE__);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef((GLfloat)pixelX, 768 - (GLfloat)pixelY, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    handleGLErrors(__FILE__, __LINE__);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    handleGLErrors(__FILE__, __LINE__);

    _StartDrawing(fce);
        float dx = _DrawText(fce, size, text);
    _EndDrawing(fce);
    handleGLErrors(__FILE__, __LINE__);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    handleGLErrors(__FILE__, __LINE__);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    handleGLErrors(__FILE__, __LINE__);

    return dx;
}

void PrintTextString(char *text, gbVec2* pos, gbVec4* color, float scale, float spacing) {
}
