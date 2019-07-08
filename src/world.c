#include "world.h"

TileData* WorldArray = NULL;

float tileSideLength;
float tileBottomDisplacement;
float tileRadius;
float tileFullHeight;
float tileFullWidth;
vec2 worldSize;
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

    worldSize[0] = tileFullWidth * width;
    worldSize[1] = (tileFullHeight * height) - (tileBottomDisplacement * (height - 1));
    if (height > 1) {
        worldSize[0] += tileRadius;
    }
    
    arrsetlen(WorldArray, width * height);
    
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            WorldArray[j*width + i].i = j*width + i;
            WorldArray[j*width + i].color[0] = rand() / (float)RAND_MAX;
            WorldArray[j*width + i].color[1] = 0.0f; // rand() / (float)RAND_MAX;
            WorldArray[j*width + i].color[2] = rand() / (float)RAND_MAX;
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
    vec2 startingPosition = {worldSize[0] * -0.5f, worldSize[1] * 0.5f};
    startingPosition[0] += tileRadius;
    startingPosition[1] += tileFullHeight * -0.5f;
    vec2 modVector;
    
    for (int j = 0; j < worldHeight; j++) {
        for (int i = 0; i < worldWidth; i++) {
            modVector[0] = tileFullWidth * i;
            modVector[1] = -((tileFullHeight - tileBottomDisplacement) * j);
            if (j % 2) {
                modVector[0] += tileRadius;
            }
            
            glPushMatrix();
                glTranslatef(
                             startingPosition[0] + modVector[0],
                             startingPosition[1] + modVector[1],
                             0.0f
                             );
                glScalef(tileFullHeight, tileFullHeight, 1.0f);
                glColor3fv(WorldArray[j*worldWidth + i].color);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
            glPopMatrix();
        }
    }
}
