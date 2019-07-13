#include "world.h"

TileData* WorldArray = NULL;

float tileSideLength;
float tileBottomDisplacement;
float tileRadius;
float tileFullHeight;
float tileFullWidth;
gbVec2 worldSize;
int worldWidth;
int worldHeight;

void InitializeWorld(int width, int height, float scale) {
    worldWidth = width;
    worldHeight = height;

    tileSideLength = scale;
    tileBottomDisplacement = sinf(M_PI / 6.0f) * tileSideLength;
    tileRadius = cosf(M_PI / 6.0f) * tileSideLength;
    tileFullHeight = tileSideLength + (2.0f * tileBottomDisplacement);
    tileFullWidth = 2.0f * tileRadius;

    worldSize.x = tileFullWidth * width;
    worldSize.y = (tileFullHeight * height) - (tileBottomDisplacement * (height - 1));
    if (height > 1) {
        worldSize.x += tileRadius;
    }

    arrsetlen(WorldArray, width * height);

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            TileData* td = &WorldArray[j*width + i];
            td->i = j*width + i;
            td->hexPos.x = i;
            td->hexPos.y = j;
            td->color.r = gb_random01(); // 0.0f;
            td->color.g = gb_random01(); // 0.0f;
            td->color.b = gb_random01(); // 0.0f;

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
}

void FinalizeWorld() {
    arrfree(WorldArray);
}

Vec2i GetWorldDimensions() {
    Vec2i dim = {worldWidth, worldHeight};
    return dim;
}

float GetWorldScale() {
    return tileSideLength;
}

TileData* GetTileAtPosition(int x, int y) {
    if (x < 0 || x >= worldWidth) {
        fprintf(stderr, "Invalid tile x index: %d\n", x);
        return NULL;
    }
    if (y < 0 || y >= worldWidth) {
        fprintf(stderr, "Invalid tile y index: %d\n", y);
        return NULL;
    }
    return &WorldArray[y*worldWidth + x];
}

TileData* GetTileAtIndex(int i) {
    if (i < 0 || i >= arrlen(WorldArray)) {
        fprintf(stderr, "Invalid tile index: %d\n", i);
        return NULL;
    }
    return &WorldArray[i];
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
    gbVec2 startingPosition = {worldSize.x * -0.5f, worldSize.y * 0.5f};
    startingPosition.x += tileRadius;
    startingPosition.y += tileFullHeight * -0.5f;
    gbVec2 modVector;

    for (int j = 0; j < worldHeight; j++) {
        for (int i = 0; i < worldWidth; i++) {
            modVector.x = tileFullWidth * i;
            modVector.y = -((tileFullHeight - tileBottomDisplacement) * j);
            if (j % 2) {
                modVector.x += tileRadius;
            }

            glPushMatrix();
                glTranslatef(
                             startingPosition.x + modVector.x,
                             startingPosition.y + modVector.y,
                             0.0f
                );
                glScalef(tileFullHeight, tileFullHeight, 1.0f);
                glColor3fv(WorldArray[j*worldWidth + i].color.e);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
            glPopMatrix();
        }
    }
}

int AddTileToSet(TileSet* ts, TileData* t) {
    hmput(ts->tiles, t, 1);
    return (int)hmlen(ts);
}

int RemoveTileFromSet(TileSet* ts, TileData* t) {
    hmdel(ts->tiles, t);
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

void DestroyTileSet(TileSet* ts) {
    hmfree(ts->tiles);
}
