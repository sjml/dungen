#include "stdafx.h"
#include "images.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


struct { char* key; gbVec3* value; } *colorTables = NULL;

unsigned char* GetRawImageData(const char* filename, unsigned int *dataLength, int *texWidth, int *texHeight) {
    int n;
    unsigned char* imageData = stbi_load(filename, texWidth, texHeight, &n, 4);
    *dataLength = (*texWidth) * (*texHeight) * 4;

    return imageData;
}


void LoadColorTable(const char* tablename, const char* filename) {
    unsigned int dataLength;
    int width, height;
    unsigned char* imageData = GetRawImageData(filename, &dataLength, &width, &height);

    gbVec3* colorList = NULL;
    arrsetlen(colorList, dataLength / 4);
    unsigned int pixOffset = 0;
    for (int y=0; y < height; y++) {
        for (int x=0; x < width; x++) {
            pixOffset = (y * width * 4) + (x * 4);
            colorList[height*y + x].r = (float)imageData[pixOffset + 0] / 255.0f;
            colorList[height*y + x].g = (float)imageData[pixOffset + 1] / 255.0f;
            colorList[height*y + x].b = (float)imageData[pixOffset + 2] / 255.0f;
        }
    }
    stbi_image_free(imageData);

    if (shget(colorTables, tablename) != NULL) {
        fprintf(stderr, "ERROR: existing color table named `%s`.\n", tablename);
        return;
    }
    shput(colorTables, tablename, colorList);
}

/*
vec3* GetColorByIndex(const char* tablename, int i) {
    vec3* ct = shget(colorTables, tablename);
    if (ct == NULL) {
        fprintf(stderr, "ERROR: no color table named `%s`.\n", tablename);
        return NULL;
    }
    if (i < 0 || i >= arrlen(ct)) {
        fprintf(stderr, "ERROR: %d is invalid index to `%s`.\n", i, tablename);
        return NULL;
    }
    return &(ct[i]);
}

vec3* GetColorByPercent(const char* tablename, float p) {
    vec3* ct = shget(colorTables, tablename);
    if (ct == NULL) {
        fprintf(stderr, "ERROR: no color table named `%s`.\n", tablename);
        return NULL;
    }
    float fIndex = p * (float)arrlen(ct);
    int indexBase = fIndex;
    float offset = fIndex - (float)indexBase;
    if (++indexBase >= arrlen(ct)) {
        return &(ct[--indexBase]);
    }

    return &(ct[indexBase]);
}

*/
