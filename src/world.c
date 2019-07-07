#include "world.h"

TileData World[worldWidth * worldHeight];

float tileSideLength;
float tileBottomDisplacement;
float tileRadius;
float tileFullHeight;
float tileFullWidth;
vec2 worldSize;

void InitializeWorld(void) {
    tileSideLength = tileScale;
    tileBottomDisplacement = sinf(M_PI / 6.0f) * tileSideLength;
    tileRadius = cosf(M_PI / 6.0f) * tileSideLength;
    tileFullHeight = tileSideLength + (2.0f * tileBottomDisplacement);
    tileFullWidth = 2.0f * tileRadius;

    worldSize[0] = tileFullWidth * worldWidth;
    worldSize[1] = (tileFullHeight * worldHeight) - (tileBottomDisplacement * (worldHeight - 1));
    if (worldHeight > 1) {
        worldSize[0] += tileRadius;
    }
    
    for (int j = 0; j < worldHeight; j++) {
        for (int i = 0; i < worldWidth; i++) {
            World[j*worldWidth + i].color[0] = rand() / (float)RAND_MAX;
            World[j*worldWidth + i].color[1] = rand() / (float)RAND_MAX;
            World[j*worldWidth + i].color[2] = rand() / (float)RAND_MAX;
        }
    }
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
                glColor3fv(World[j*worldWidth + i].color);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
            glPopMatrix();
        }
    }
}
