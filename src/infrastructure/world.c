#include "stdafx.h"
#include "world.h"

#include "attributes.h"
#include "outline.h"
#include "rendering.h"
#include "game.h"

static const float hexVertices[] = {
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



static TileData* WorldArray = NULL;
static TileSet* dirtyTiles = NULL;
static struct { Region* key; int value; }* dirtyRegions = NULL;
static gbVec2** PointList = NULL;

static float tileSize;
static gbVec2 tileDimensions;
static gbVec2 worldSize;
static Vec2i tileWorldSize;

static Agent** agents;


void InitializeWorld(int width, int height, float scale) {
    tileWorldSize.x = width;
    tileWorldSize.y = height;

    tileSize = scale;
    tileDimensions.y = 2.0f * scale;
    tileDimensions.x = sqrtf(3.0f) * scale;

    worldSize.x = tileDimensions.x * width;
    worldSize.y = (tileDimensions.y * height) - ((tileSize * 0.5f) * (height - 1));
    if (height > 1) {
        worldSize.x += tileDimensions.x / 2.0f;
    }

    arrsetlen(WorldArray, (unsigned int)(width * height));
    arrsetlen(PointList, (unsigned int)(height+1));

    gbVec2 startingPos = { worldSize.x * -0.5f, worldSize.y * 0.5f };
    float yHigh;
    float yLow;
    float currentX;
    bool high = true;
    for (int j = 0; j < height + 1; j++) {
        PointList[j] = NULL;
        arrsetlen(PointList[j], (unsigned int)(2 * (width+1)));

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
            td->memberRegions = NULL;
            td->hexPos.x = i;
            td->hexPos.y = j;
//            td->color.r = RandomRangeFloat(0.0f, 1.0f);
//            td->color.g = RandomRangeFloat(0.0f, 1.0f);
//            td->color.b = RandomRangeFloat(0.0f, 1.0f);
            td->color.r = 0.0f;
            td->color.g = 1.0f;
            td->color.b = 1.0f;
            td->overlayColor.r = 0.0f;
            td->overlayColor.g = 0.0f;
            td->overlayColor.b = 0.0f;
            td->overlayColor.a = 0.0f;
            td->outline = NULL;

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
    Vec2i dim = {tileWorldSize.x, tileWorldSize.y};
    return dim;
}

float GetWorldScale() {
    return tileSize;
}

gbVec2** GetWorldPointList() {
    return PointList;
}

TileData** GetAllTiles() {
    TileData** ret = NULL;
    arrsetlen(ret, arrlenu(WorldArray));
    for (long long i=0; i < arrlen(WorldArray); i++) {
        ret[i] = &WorldArray[i];
    }
    return ret;
}

TileSet* GetAllTilesAsSet(void) {
    TileSet* ret = NULL;
    for (long long i=0; i < arrlen(WorldArray); i++) {
        ret = AddTileToSet(ret, &WorldArray[i]);
    }
    return ret;
}

void SetTileAsDirty(TileData* td) {
    dirtyTiles = AddTileToSet(dirtyTiles, td);
}

TileData** GetDirtyTiles(void) {
    return GetTilesFromSet(dirtyTiles);
}

void CleanAllTiles(void) {
    hmfree(dirtyTiles);
    dirtyTiles = NULL;
}

void SetRegionAsDirty(Region* r) {
    hmput(dirtyRegions, r, 1);
    for (long i = 0; i < hmlen(r->tiles); i++) {
        SetTileAsDirty(r->tiles[i].key);
    }
}

Region** GetDirtyRegions(void) {
    Region** ret = NULL;
    arrsetlen(ret, hmlenu(dirtyRegions));
    for (long i=0; i < hmlen(dirtyRegions); i++) {
        ret[i] = dirtyRegions[i].key;
    }
    return ret;
}

void CleanAllRegions(void) {
    hmfree(dirtyRegions);
    dirtyRegions = NULL;
}

TileSet* IntersectTileSets(TileSet* set1, TileSet* set2) {
    TileSet* ret = NULL;

    for (long i=0; i < hmlen(set1); i++) {
        if (IsTileInSet(set2, set1[i].key)) {
            ret = AddTileToSet(ret, set1[i].key);
        }
    }

    return ret;
}

TileData* GetTileAtPosition(int x, int y) {
    if (x < 0 || x >= tileWorldSize.x) {
        fprintf(stderr, "Invalid tile x index: %d\n", x);
        return NULL;
    }
    if (y < 0 || y >= tileWorldSize.x) {
        fprintf(stderr, "Invalid tile y index: %d\n", y);
        return NULL;
    }
    return &WorldArray[y*tileWorldSize.x + x];
}

TileData* GetTileAtIndex(long long i) {
    if (i < 0 || i >= arrlen(WorldArray)) {
//        fprintf(stderr, "Invalid tile index: %lld\n", i);
        return NULL;
    }
    return &WorldArray[i];
}

// this is probably not as efficient as it could be, but not the bottleneck
TileData* ScreenToTile(gbVec2* screenCoordinates) {
    Vec2i winDims = GetWindowDimensions();
    if (   screenCoordinates->x < 0 || screenCoordinates->y < 0
        || screenCoordinates->x > winDims.x || screenCoordinates->y > winDims.y
    ) {
        return NULL;
    }

    const float aspect = (float)winDims.x / (float)winDims.y;
    const float height = 20.0f;
    const float width = height * aspect;
    const float ratio = width / (float)winDims.x;
    Vec2i sizePx = { (int)(worldSize.x / ratio), (int)(worldSize.y / ratio) };
    Vec2i offset = { (int)(winDims.x - sizePx.x), (int)(winDims.y - sizePx.y) };
    offset.x /= 2;
    offset.y /= 2;

    Vec2i absCoord = { (int)(screenCoordinates->x - offset.x), (int)(screenCoordinates->y - offset.y) };
    const float sideSlope = (float)tan(GB_MATH_PI / 6.0);
    gbVec2 absCoordW = {absCoord.x * ratio, absCoord.y * ratio};

    gbVec2 sectionCoordW = {absCoordW.x / tileDimensions.x, absCoordW.y / ((tileSize * 0.5f) + tileSize) };
    Vec2i sectionCoord = { (int)sectionCoordW.x, (int)sectionCoordW.y };

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

    if (hexCoord.x < 0 || hexCoord.x >= tileWorldSize.x || hexCoord.y < 0 || hexCoord.y >= tileWorldSize.y) {
        return NULL;
    }

    return GetTileAtPosition(hexCoord.x, hexCoord.y);
}

TileData** GetTileNeighbors(TileData* center) {
    TileData** neighbors = NULL;
    if (center->neighborW != -1) {
        arrpush(neighbors, &WorldArray[center->neighborW]);
    }
    if (center->neighborNW != -1) {
        arrpush(neighbors, &WorldArray[center->neighborNW]);
    }
    if (center->neighborNE != -1) {
        arrpush(neighbors, &WorldArray[center->neighborNE]);
    }
    if (center->neighborE != -1) {
        arrpush(neighbors, &WorldArray[center->neighborE]);
    }
    if (center->neighborSE != -1) {
        arrpush(neighbors, &WorldArray[center->neighborSE]);
    }
    if (center->neighborSW != -1) {
        arrpush(neighbors, &WorldArray[center->neighborSW]);
    }

    return neighbors;
}

TileData** GetTileCircle(TileData* center, int radius) {
    TileSet* ts = NULL;
    ts = AddTileToSet(ts, center);

    int count = 0;
    TileData** expansion = NULL;
    while (count++ < radius) {
        for (long i = 0; i < hmlen(ts); i++) {
            TileData** neighbors = GetTileNeighbors(ts[i].key);
            for (long n = 0; n < arrlen(neighbors); n++) {
                arrpush(expansion, neighbors[n]);
            }
            arrfree(neighbors);
        }
        for (long e = 0; e < arrlen(expansion); e++) {
            ts = AddTileToSet(ts, expansion[e]);
        }
        arrfree(expansion);
    }

    TileData** ret = GetTilesFromSet(ts);
    DestroyTileSet(ts);
    return ret;
}

Agent* GetTileOwner(TileData* t) {
    Region** regionsToCheck = NULL;
    for (long long i = 0; i < arrlen(t->memberRegions); i++) {
        arrpush(regionsToCheck, t->memberRegions[i]);
    }
    for (long long i = 0; i < arrlen(regionsToCheck); i++) {
        if (regionsToCheck[i]->parent != NULL) {
            arrpush(regionsToCheck, regionsToCheck[i]->parent);
        }
        int agentIdx = GetRegionAttributeInt(regionsToCheck[i], "domainOwner");
        if (agentIdx > 0) {
            arrfree(regionsToCheck);
            return agents[agentIdx - 1];
        }
    }
    arrfree(regionsToCheck);
    return NULL;
}

void RenderTiles(void) {
    glVertexPointer(2, GL_FLOAT, 0, hexVertices);

    for (long long i = 0; i < arrlen(WorldArray); i++) {
        glPushMatrix();
        glTranslatef(
            WorldArray[i].worldPos.x,
            WorldArray[i].worldPos.y,
            0.0f
        );
        glScalef(tileDimensions.y, tileDimensions.y, 1.0f);
        glColor4f(WorldArray[i].color.r, WorldArray[i].color.g, WorldArray[i].color.b, 1.0f);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
        if (WorldArray[i].overlayColor.a > 0.0f) {
            glColor4f(WorldArray[i].overlayColor.r, WorldArray[i].overlayColor.g, WorldArray[i].overlayColor.b, WorldArray[i].overlayColor.a);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
        }
        glPopMatrix();
    }
    for (long i = 0; i < arrlen(WorldArray); i++) {
        if (WorldArray[i].outline != NULL) {
            RenderOutline(WorldArray[i].outline);
        }
    }
}

Region* CreateRegion() {
    Region* r = malloc(sizeof(Region));
    r->tiles = NULL;
    r->outline = NULL;
    r->label.text = NULL;
    r->label.scale = -1.0f;
    r->children = NULL;
    r->parent = NULL;
    AddRegionToRendering(r);
    r->i = SetupRegionAttributeData(r);
    return r;
}

void DestroyRegion(Region* r) {
    SetRegionParent(r, NULL);
    for (long i=0; i < arrlen(r->children); i++) {
        r->children[i]->parent = NULL;
    }
    arrfree(r->children);

    ClearRegionAttributeData(r);
    RemoveRegionFromRendering(r);
    if (r->outline != NULL) {
        DestroyOutline(r->outline);
    }
    if (r->label.scale >= 0.0f) {
        ClearRegionLabel(r);
    }
    for (int i = 0; i < hmlen(r->tiles); i++) {
        RemoveTileFromRegion(r, r->tiles[i].key);
    }
    hmfree(r->tiles);
    free(r);
}

void AddTileToRegion(Region* r, TileData* t) {
    long len = hmlen(r->tiles);
    r->tiles = AddTileToSet(r->tiles, t);
    if (len != hmlen(r->tiles)) {
        arrpush(t->memberRegions, r);
    }
}

void RemoveTileFromRegion(Region* r, TileData* t) {
    r->tiles = RemoveTileFromSet(r->tiles, t);
    for (long i=0; i < arrlen(t->memberRegions); i++) {
        if (t->memberRegions[i] == r) {
            arrdel(t->memberRegions, i);
            break;
        }
    }
}

void SetRegionParent(Region* child, Region* newParent) {
    if (child->parent != NULL) {
        for (long i=0; i < arrlen(child->parent->children); i++) {
            if (child->parent->children[i] == child) {
                arrdel(child->parent->children, i);
                break;
            }
        }
    }
    child->parent = newParent;
    if (newParent != NULL) {
        arrpush(newParent->children, child);
    }
}

Region* GetRegionParent(Region* r) {
    return r->parent;
}

Region** GetRegionChildren(Region* r) {
    return r->children;
}

void DestroyTileSet(TileSet* ts) {
    hmfree(ts);
}

TileSet* AddTileToSet(TileSet* ts, TileData* t) {
    hmput(ts, t, 1);
    return ts;
}

TileSet* RemoveTileFromSet(TileSet* ts, TileData* t) {
    hmdel(ts, t);
    return ts;
}

bool IsTileInSet(TileSet* ts, TileData* t) {
    return hmgeti(ts, t) >= 0;
}

long GetTileSetCount(TileSet* ts) {
    return hmlen(ts);
}

TileData** GetTilesFromSet(TileSet* ts) {
    TileData** ret = NULL;
    arrsetlen(ret, (unsigned int)hmlen(ts));
    for (int i=0; i < hmlen(ts); i++) {
        ret[i] = ts[i].key;
    }

    return ret;
}

void SetTileOutline(TileData* t, gbVec4 color, float thickness, int type) {
    if (t->outline != NULL) {
        ClearTileOutline(t);
    }
    TileSet* singleTile = NULL;
    hmput(singleTile, t, 1);
    t->outline = CreateOutline(singleTile, thickness, type);
    t->outline->color.r = color.r;
    t->outline->color.g = color.g;
    t->outline->color.b = color.b;
    t->outline->color.a = color.a;
    DestroyTileSet(singleTile);
}

void ClearTileOutline(TileData* t) {
    if (t->outline!= NULL) {
        DestroyOutline(t->outline);
        t->outline = NULL;
    }
}

void SetRegionOutline(Region* r, gbVec4 color, float thickness, int type) {
    if (r->outline != NULL) {
        ClearRegionOutline(r);
    }
    r->outline = CreateOutline(r->tiles, thickness, type);
    r->outline->color.r = color.r;
    r->outline->color.g = color.g;
    r->outline->color.b = color.b;
    r->outline->color.a = color.a;
}

void ClearRegionOutline(Region* r) {
    if (r->outline != NULL) {
        DestroyOutline(r->outline);
        r->outline = NULL;
    }
}

void SetRegionLabel(Region* r, const char* text, float scale, gbVec4 color, gbVec2 tileOffset) {
    r->label.text = malloc(sizeof(char) * (strlen(text) + 1));
    strcpy(r->label.text, text);
    r->label.scale = scale;
    r->label.color.r = color.r;
    r->label.color.g = color.g;
    r->label.color.b = color.b;
    r->label.color.a = color.a;

    gbVec2 min = {  FLT_MAX,  FLT_MAX };
    gbVec2 max = { -FLT_MAX, -FLT_MAX };
    for (long i=0; i < hmlen(r->tiles); i++) {
        min.x = gb_min(min.x, r->tiles[i].key->worldPos.x);
        min.y = gb_min(min.y, r->tiles[i].key->worldPos.y);
        max.x = gb_max(max.x, r->tiles[i].key->worldPos.x);
        max.y = gb_max(max.y, r->tiles[i].key->worldPos.y);
    }

    gbVec2 center = {
        (max.x - min.x) * 0.5f,
        (max.y - min.y) * 0.5f
    };
    gb_vec2_lerp(&center, min, max, 0.5f);
    center.x += tileDimensions.x * (float)tileOffset.x;
    center.y += tileDimensions.y * (float)tileOffset.y;

    gbVec2 screenPos = WorldToScreen(center);
    gbVec2 orthoPos = ScreenToOrtho(screenPos);

    gbVec2 extents = MeasureTextExtents(text, "fonts/04B_03__.TTF", scale);
    r->label.pos.x = orthoPos.x - (extents.x * 0.5f) + 1.0f;
    r->label.pos.y = orthoPos.y + (extents.y * 0.5f);
}

void ClearRegionLabel(Region* r) {
    free(r->label.text);
    r->label.scale = -1.0f;
}


Agent* CreateAgent(void) {
    Agent* a = malloc(sizeof(Agent));

    a->domain = CreateRegion();
    arrpush(agents, a);
    a->i = arrlen(agents);
    SetRegionAttributeInt(a->domain, "domainOwner", (int)a->i);

    SetupAgentAttributeData(a);

    return a;
}

Agent** GetAllAgents(void) {
    return agents;
}
