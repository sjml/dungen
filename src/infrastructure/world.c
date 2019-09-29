#include "stdafx.h"
#include "world.h"

#include "attributes.h"
#include "outline.h"
#include "rendering.h"
#include "game.h"


static TileDrawData* TileDrawArray = NULL;
static TileMetaData* TileMetaArray = NULL;
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

    arrsetlen(TileDrawArray, (unsigned int)(width * height));
    arrsetlen(TileMetaArray, (unsigned int)(width * height));
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
            td->draw = &TileDrawArray[j*width + i];
            td->meta = &TileMetaArray[j*width + i];

            td->meta->i = j*width + i;
            td->meta->memberRegions = NULL;
            td->meta->hexPos.x = i;
            td->meta->hexPos.y = j;
            td->draw->color.r = RandomRangeFloat(0.0f, 1.0f);
            td->draw->color.g = RandomRangeFloat(0.0f, 1.0f);
            td->draw->color.b = RandomRangeFloat(0.0f, 1.0f);
//            td->draw->color.r = 0.0f;
//            td->draw->color.g = 1.0f;
//            td->draw->color.b = 1.0f;
            td->draw->overlayColor.r = 0.0f;
            td->draw->overlayColor.g = 0.0f;
            td->draw->overlayColor.b = 0.0f;
            td->draw->overlayColor.a = 0.0f;
            td->meta->outline = NULL;

            modVector.x = tileDimensions.x * i;
            modVector.y = -((tileDimensions.y - (tileSize * 0.5f)) * j);
            if (j % 2) {
                modVector.x += tileDimensions.x * 0.5f;
            }
            gb_vec2_add(&td->draw->worldPos, startingPos, modVector);

            td->meta->neighborE = j * width + i + 1;
            td->meta->neighborW = j * width + i - 1;
            if (j % 2) {
                td->meta->neighborNW = (j-1) * width + i;
                td->meta->neighborNE = (j-1) * width + i + 1;
                td->meta->neighborSW = (j+1) * width + i;
                td->meta->neighborSE = (j+1) * width + i + 1;
            }
            else {
                td->meta->neighborNW = (j-1) * width + i - 1;
                td->meta->neighborNE = (j-1) * width + i;
                td->meta->neighborSW = (j+1) * width + i - 1;
                td->meta->neighborSE = (j+1) * width + i;
            }
            if (i == 0) {
                td->meta->neighborW = -1;
                if (!(j % 2)) {
                    td->meta->neighborNW = -1;
                    td->meta->neighborSW = -1;
                }
            }
            if (i == width - 1) {
                td->meta->neighborE = -1;
                if (j % 2) {
                    td->meta->neighborNE = -1;
                    td->meta->neighborSE = -1;
                }
            }
            if (j == 0) {
                td->meta->neighborNW = -1;
                td->meta->neighborNE = -1;
            }
            if (j == height - 1) {
                td->meta->neighborSW = -1;
                td->meta->neighborSE = -1;
            }
        }
    }

    SetupTileAttributeData(WorldArray);
}

long GetTileDistance(TileData* t1, TileData* t2) {
    long dx = abs(t1->meta->hexPos.x - t2->meta->hexPos.x);
    long dy = abs(t1->meta->hexPos.y - t2->meta->hexPos.y);

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

gbVec2 GetTileDimensions() {
    return tileDimensions;
}

float GetWorldScale() {
    return tileSize;
}

gbVec2** GetWorldPointList() {
    return PointList;
}

TileData* GetTileStartPointer(void) {
    return &WorldArray[0];
}

long long GetNumberOfTiles(void) {
    return arrlen(WorldArray);
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
    UpdateRenderBuffers(dirtyTiles);

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
    if (center->meta->neighborW != -1) {
        arrpush(neighbors, &WorldArray[center->meta->neighborW]);
    }
    if (center->meta->neighborNW != -1) {
        arrpush(neighbors, &WorldArray[center->meta->neighborNW]);
    }
    if (center->meta->neighborNE != -1) {
        arrpush(neighbors, &WorldArray[center->meta->neighborNE]);
    }
    if (center->meta->neighborE != -1) {
        arrpush(neighbors, &WorldArray[center->meta->neighborE]);
    }
    if (center->meta->neighborSE != -1) {
        arrpush(neighbors, &WorldArray[center->meta->neighborSE]);
    }
    if (center->meta->neighborSW != -1) {
        arrpush(neighbors, &WorldArray[center->meta->neighborSW]);
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
    for (long long i = 0; i < arrlen(t->meta->memberRegions); i++) {
        arrpush(regionsToCheck, t->meta->memberRegions[i]);
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
//    glVertexPointer(2, GL_FLOAT, 0, hexVertices);
//
//    for (long long i = 0; i < arrlen(WorldArray); i++) {
//        glPushMatrix();
//        glTranslatef(
//            WorldArray[i].draw->worldPos.x,
//            WorldArray[i].draw->worldPos.y,
//            0.0f
//        );
//        glScalef(tileDimensions.y, tileDimensions.y, 1.0f);
//        glColor4f(WorldArray[i].draw->color.r, WorldArray[i].draw->color.g, WorldArray[i].draw->color.b, 1.0f);
//        glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
//        if (WorldArray[i].draw->overlayColor.a > 0.0f) {
//            glColor4f(WorldArray[i].draw->overlayColor.r, WorldArray[i].draw->overlayColor.g, WorldArray[i].draw->overlayColor.b, WorldArray[i].draw->overlayColor.a);
//            glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
//        }
//        glPopMatrix();
//    }
//    for (long i = 0; i < arrlen(WorldArray); i++) {
//        if (WorldArray[i].meta->outline != NULL) {
//            RenderOutline(WorldArray[i].meta->outline);
//        }
//    }
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

Region* GetRegionByIndex(long long idx) {
    Region** regions = GetRenderingRegions();
    Region* ret = NULL;
    for (long long i = 0; i < arrlen(regions); i++) {
        if (regions[i]->i == idx) {
            ret = regions[i];
            break;
        }
    }
    arrfree(regions);
    return ret;
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
        arrpush(t->meta->memberRegions, r);
    }
}

void RemoveTileFromRegion(Region* r, TileData* t) {
    r->tiles = RemoveTileFromSet(r->tiles, t);
    for (long i=0; i < arrlen(t->meta->memberRegions); i++) {
        if (t->meta->memberRegions[i] == r) {
            arrdel(t->meta->memberRegions, i);
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
    if (t == NULL) {
        return ts;
    }
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
    if (t->meta->outline != NULL) {
        ClearTileOutline(t);
    }
    TileSet* singleTile = NULL;
    hmput(singleTile, t, 1);
    t->meta->outline = CreateOutline(singleTile, thickness, type);
    t->meta->outline->color.r = color.r;
    t->meta->outline->color.g = color.g;
    t->meta->outline->color.b = color.b;
    t->meta->outline->color.a = color.a;
    DestroyTileSet(singleTile);
}

void ClearTileOutline(TileData* t) {
    if (t->meta->outline!= NULL) {
        DestroyOutline(t->meta->outline);
        t->meta->outline = NULL;
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

gbVec2 GetRegionCenterPoint(Region* r) {
    gbVec2 center = { 0.0f, 0.0f };
    if (GetTileSetCount(r->tiles) == 0) {
        return center;
    }

    gbVec2 min = {  FLT_MAX,  FLT_MAX };
    gbVec2 max = { -FLT_MAX, -FLT_MAX };
    for (long i=0; i < hmlen(r->tiles); i++) {
        min.x = gb_min(min.x, r->tiles[i].key->draw->worldPos.x);
        min.y = gb_min(min.y, r->tiles[i].key->draw->worldPos.y);
        max.x = gb_max(max.x, r->tiles[i].key->draw->worldPos.x);
        max.y = gb_max(max.y, r->tiles[i].key->draw->worldPos.y);
    }

    gb_vec2_lerp(&center, min, max, 0.5f);
    return center;
}

void SetRegionLabel(Region* r, const char* text, float scale, gbVec4 color, gbVec2 tileOffset) {
    if (r->label.text != NULL) {
        free(r->label.text);
    }
    r->label.text = _strdup(text);
    r->label.scale = scale;
    r->label.color.r = color.r;
    r->label.color.g = color.g;
    r->label.color.b = color.b;
    r->label.color.a = color.a;

    gbVec2 center = GetRegionCenterPoint(r);

    center.x += tileDimensions.x * (float)tileOffset.x;
    center.y += tileDimensions.y * (float)tileOffset.y;

    gbVec2 screenPos = WorldToScreen(center);
    gbVec2 orthoPos = ScreenToOrtho(screenPos);

    gbVec2 extents = MeasureTextExtents(text, "Pixel", scale);
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

// this *should* be as simple as just an array lookup, but with my luck
//   I'll change it so agents can be deleted and forget to update this
//   so trading some inefficiency for protection against future buffoonery.
Agent* GetAgentByIndex(long long idx) {
    for (long long i = 0; i < arrlen(agents); i++) {
        if (agents[i]->i == idx) {
            return agents[i];
        }
    }
    return NULL;
}

Agent** GetAllAgents(void) {
    return agents;
}
