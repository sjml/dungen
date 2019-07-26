#include "../stdafx.h"
#include "world.h"

#include "attributes.h"
#include "outline.h"
#include "rendering.h"
#include "util.h"

const float hexVertices[] = {
    0.0f,               0.0f,
    0.5f * -0.8660254f, 0.5f *  0.5f,
    0.5f * -0.8660254f, 0.5f * -0.5f,
    0.5f *  0.0000000f, 0.5f * -1.0f,
    0.5f *  0.8660254f, 0.5f * -0.5f,
    0.5f *  0.8660254f, 0.5f *  0.5f,
    0.5f *  0.0f,       0.5f *  1.0f,
    0.5f * -0.8660254f, 0.5f *  0.5f,
};

//const float hexTexVertices[] = {
//    hexVertices[0]  + 0.5f, hexVertices[1]  + 0.5f,
//    hexVertices[2]  + 0.5f, hexVertices[3]  + 0.5f,
//    hexVertices[4]  + 0.5f, hexVertices[5]  + 0.5f,
//    hexVertices[6]  + 0.5f, hexVertices[7]  + 0.5f,
//    hexVertices[8]  + 0.5f, hexVertices[9]  + 0.5f,
//    hexVertices[10] + 0.5f, hexVertices[11] + 0.5f,
//    hexVertices[12] + 0.5f, hexVertices[13] + 0.5f,
//    hexVertices[14] + 0.5f, hexVertices[15] + 0.5f,
//};



TileData* WorldArray = NULL;
gbVec2** PointList = NULL;

float tileSize;
gbVec2 tileDimensions;
gbVec2 worldSize;
Vec2i tileSetSize;

void InitializeWorld(int width, int height, float scale) {
    tileSetSize.x = width;
    tileSetSize.y = height;

    tileSize = scale;
    tileDimensions.y = 2.0f * scale;
    tileDimensions.x = sqrtf(3.0f) * scale;

    worldSize.x = tileDimensions.x * width;
    worldSize.y = (tileDimensions.y * height) - ((tileSize * 0.5f) * (height - 1));
    if (height > 1) {
        worldSize.x += tileDimensions.x / 2.0f;
    }

    arrsetlen(WorldArray, width * height);
    arrsetlen(PointList, height+1);

    gbVec2 startingPos = { worldSize.x * -0.5f, worldSize.y * 0.5f };
    float yHigh;
    float yLow;
    float currentX;
    bool high = true;
    for (int j = 0; j < height + 1; j++) {
        PointList[j] = NULL;
        arrsetlen(PointList[j], 2 * (width+1));

        high = (j % 2 != 0);
        yHigh = startingPos.y - (j * ((tileSize * 0.5f) + tileSize));
        yLow = yHigh - (tileSize * 0.5f);
        currentX = startingPos.x;

        for (int i = 0; i < arrlen(PointList[j]); i++) {
            PointList[j][i].x = currentX;
            if (high) {
                PointList[j][i].y = yHigh;
            }
            else {
                PointList[j][i].y = yLow;
            }
            currentX += tileDimensions.x * 0.5f;
            high = !high;
        }
    }

    startingPos.x = (worldSize.x * -0.5f) + (tileDimensions.x  *  0.5f);
    startingPos.y = (worldSize.y *  0.5f) + (tileDimensions.y * -0.5f);
    gbVec2 modVector;

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            TileData* td = &WorldArray[j*width + i];
            td->i = j*width + i;
            td->memberSets = NULL;
            td->hexPos.x = i;
            td->hexPos.y = j;
            td->color.r = RandomRangeFloat(0.0f, 1.0f); // 0.0f;
            td->color.g = RandomRangeFloat(0.0f, 1.0f); // 0.0f;
            td->color.b = RandomRangeFloat(0.0f, 1.0f); // 0.0f;

            modVector.x = tileDimensions.x * i;
            modVector.y = -((tileDimensions.y - (tileSize * 0.5f)) * j);
            if (j % 2) {
                modVector.x += tileDimensions.x * 0.5f;
            }
            gb_vec2_add(&td->worldPos, startingPos, modVector);

            td->neighborE = j * width + i + 1;
            td->neighborW = j * width + i - 1;
            if (j % 2) {
                td->neighborNW = (j-1) * width + i;
                td->neighborNE = (j-1) * width + i + 1;
                td->neighborSW = (j+1) * width + i;
                td->neighborSE = (j+1) * width + i + 1;
            }
            else {
                td->neighborNW = (j-1) * width + i - 1;
                td->neighborNE = (j-1) * width + i;
                td->neighborSW = (j+1) * width + i - 1;
                td->neighborSE = (j+1) * width + i;
            }
            if (i == 0) {
                td->neighborW = -1;
                if (!(j % 2)) {
                    td->neighborNW = -1;
                    td->neighborSW = -1;
                }
            }
            if (i == width - 1) {
                td->neighborE = -1;
                if (j % 2) {
                    td->neighborNE = -1;
                    td->neighborSE = -1;
                }
            }
            if (j == 0) {
                td->neighborNW = -1;
                td->neighborNE = -1;
            }
            if (j == height - 1) {
                td->neighborSW = -1;
                td->neighborSE = -1;
            }
        }
    }

    SetupTileAttributeData(WorldArray);
}

long GetTileDistance(TileData* t1, TileData* t2) {
    long dx = abs(t1->hexPos.x - t2->hexPos.x);
    long dy = abs(t1->hexPos.y - t2->hexPos.y);
    
    return dx + dy;
}

void FinalizeWorld() {
    for (int i=0; i < arrlen(PointList); i++) {
        arrfree(PointList[i]);
    }
    arrfree(PointList);
    arrfree(WorldArray);
}

Vec2i GetWorldDimensions() {
    Vec2i dim = {tileSetSize.x, tileSetSize.y};
    return dim;
}

float GetWorldScale() {
    return tileSize;
}

gbVec2** GetWorldPointList() {
    return PointList;
}

TileData* GetTileAtPosition(int x, int y) {
    if (x < 0 || x >= tileSetSize.x) {
        fprintf(stderr, "Invalid tile x index: %d\n", x);
        return NULL;
    }
    if (y < 0 || y >= tileSetSize.x) {
        fprintf(stderr, "Invalid tile y index: %d\n", y);
        return NULL;
    }
    return &WorldArray[y*tileSetSize.x + x];
}

TileData* GetTileAtIndex(long long i) {
    if (i < 0 || i >= arrlen(WorldArray)) {
        fprintf(stderr, "Invalid tile index: %lld\n", i);
        return NULL;
    }
    return &WorldArray[i];
}

// this is probably not as efficient as it could be, but not the bottleneck
TileData* ScreenToTile(gbVec2* screenCoordinates) {
    if (   screenCoordinates->x < 0 || screenCoordinates->y < 0
        || screenCoordinates->x > 1024.0f || screenCoordinates->y > 768.0f
    ) {
        return NULL;
    }

    const static float aspect = 1024.0f / 768.0f;
    const static float height = 20.0f;
    const static float width = height * aspect;
    const static float ratio = width / 1024.0f;
    Vec2i sizePx = { worldSize.x / ratio, worldSize.y / ratio };
    Vec2i offset = { 1024.0f - sizePx.x, 768.0f - sizePx.y };
    offset.x /= 2;
    offset.y /= 2;

    Vec2i absCoord = {screenCoordinates->x - offset.x, screenCoordinates->y - offset.y};
    const float sideSlope = tan((float)M_PI / 6.0f);
    gbVec2 absCoordW = {absCoord.x * ratio, absCoord.y * ratio};

    gbVec2 sectionCoordW = {absCoordW.x / tileDimensions.x, absCoordW.y / ((tileSize * 0.5f) + tileSize) };
    Vec2i sectionCoord = { sectionCoordW.x, sectionCoordW.y };

    gbVec2 remainders = {sectionCoordW.x - sectionCoord.x, sectionCoordW.y - sectionCoord.y };
    gbVec2 localCoordW = {remainders.x * tileDimensions.x, remainders.y * ((tileSize * 0.5f) + tileSize) };

    Vec2i hexCoord;
    if ((sectionCoord.y & 1) == 0) {
        hexCoord.x = sectionCoord.x;
        hexCoord.y = sectionCoord.y;
        if (localCoordW.y < (tileSize * 0.5f)) {
            if (localCoordW.y < ((tileSize * 0.5f) - localCoordW.x * sideSlope)) {
                hexCoord.x -= 1;
                hexCoord.y -= 1;
            }

            if (localCoordW.y < (-(tileSize * 0.5f) + localCoordW.x * sideSlope)) {
                hexCoord.y -= 1;
            }
        }
    }
    else {
        if (localCoordW.x >= (tileDimensions.x * 0.5f)) {
            if (localCoordW.y < (2 * (tileSize * 0.5f) - localCoordW.x * sideSlope)) {
                hexCoord.x = sectionCoord.x;
                hexCoord.y = sectionCoord.y - 1;
            }
            else {
                hexCoord = sectionCoord;
            }
        }
        else {
            if (localCoordW.y < (localCoordW.x * sideSlope)) {
                hexCoord.x = sectionCoord.x;
                hexCoord.y = sectionCoord.y - 1;
            }
            else  {
                hexCoord.x = sectionCoord.x - 1;
                hexCoord.y = sectionCoord.y;
            }
        }
    }

    if (hexCoord.x < 0 || hexCoord.x >= tileSetSize.x || hexCoord.y < 0 || hexCoord.y >= tileSetSize.y) {
        return NULL;
    }

    return GetTileAtPosition(hexCoord.x, hexCoord.y);
}

TileData** GetTileNeighbors(TileData* center, int *numNeighbors) {
    TileData** neighbors = malloc(sizeof(TileData*) * 6);
    (*numNeighbors) = 0;
    if (center->neighborW != -1) {
        neighbors[(*numNeighbors)++] = GetTileAtIndex(center->neighborW);
    }
    if (center->neighborNW != -1) {
        neighbors[(*numNeighbors)++] = GetTileAtIndex(center->neighborNW);
    }
    if (center->neighborNE != -1) {
        neighbors[(*numNeighbors)++] = GetTileAtIndex(center->neighborNE);
    }
    if (center->neighborE != -1) {
        neighbors[(*numNeighbors)++] = GetTileAtIndex(center->neighborE);
    }
    if (center->neighborSE != -1) {
        neighbors[(*numNeighbors)++] = GetTileAtIndex(center->neighborSE);
    }
    if (center->neighborSW != -1) {
        neighbors[(*numNeighbors)++] = GetTileAtIndex(center->neighborSW);
    }
    return neighbors;
}

void RenderTiles(void) {
    glVertexPointer(2, GL_FLOAT, 0, hexVertices);

    for (int j = 0; j < tileSetSize.y; j++) {
        for (int i = 0; i < tileSetSize.x; i++) {
            glPushMatrix();
                glTranslatef(
                             WorldArray[j*tileSetSize.x + i].worldPos.x,
                             WorldArray[j*tileSetSize.x + i].worldPos.y,
                             0.0f
                );
                glScalef(tileDimensions.y, tileDimensions.y, 1.0f);
                glColor3fv(WorldArray[j*tileSetSize.x + i].color.e);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
            glPopMatrix();
        }
    }
}

TileSet* CreateTileSet() {
    TileSet* ts = malloc(sizeof(TileSet));
    ts->tiles = NULL;
    ts->outline = NULL;
    AddTileSetToRendering(ts);
    ts->i = SetupTileSetAttributeData(ts);
    return ts;
}

void DestroyTileSet(TileSet* ts) {
    ClearTileSetAttributeData(ts);
    RemoveTileSetFromRendering(ts);
    if (ts->outline != NULL) {
        DestroyOutline(ts->outline);
    }
    for (int i = 0; i < hmlen(ts->tiles); i++) {
        RemoveTileFromSet(ts, ts->tiles[i].key);
    }
    hmfree(ts->tiles);
    free(ts);
}

int AddTileToSet(TileSet* ts, TileData* t) {
    long len = hmlen(ts->tiles);
    hmput(ts->tiles, t, 1);
    if (len != hmlen(ts->tiles)) {
        arrpush(t->memberSets, ts);
    }
    return (int)hmlen(ts);
}

int RemoveTileFromSet(TileSet* ts, TileData* t) {
    hmdel(ts->tiles, t);
    for (int i = 0; i < arrlen(t->memberSets); i++) {
        if (t->memberSets[i] == ts) {
            arrdel(t->memberSets, i);
            break;
        }
    }
    return (int)hmlen(ts);
}

bool IsTileInSet(TileSet* ts, TileData* t) {
    return hmgeti(ts->tiles, t) >= 0;
}

int GetTileCount(TileSet* ts) {
    return (int)hmlen(ts->tiles);
}

TileData** GetTiles(TileSet* ts) {
    TileData** ret = NULL;
    arrsetlen(ret, hmlen(ts->tiles));
    for (int i=0; i < hmlen(ts->tiles); i++) {
        ret[i] = ts->tiles[i].key;
    }

    return ret;
}

void SetTileSetOutline(TileSet* ts, gbVec4* color, float thickness) {
    if (ts->outline == NULL) {
        ts->outline = CreateOutline(ts, thickness);
    }
    ts->outline->color.r = color->r;
    ts->outline->color.g = color->g;
    ts->outline->color.b = color->b;
    ts->outline->color.a = color->a;
}

void ClearTileSetOutline(TileSet* ts) {
    if (ts->outline != NULL) {
        DestroyOutline(ts->outline);
        ts->outline = NULL;
    }
}
