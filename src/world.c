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
            WorldArray[j*width + i].i = j*width + i;
            WorldArray[j*width + i].color.r = gb_random01(); // 0.0f;
            WorldArray[j*width + i].color.g = gb_random01(); // 0.0f;
            WorldArray[j*width + i].color.b = gb_random01(); // 0.0f;
        }
    }
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
