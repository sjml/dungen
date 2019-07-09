#include "world.h"

TileData* WorldArray = NULL;

float tileSideLength;
float tileBottomDisplacement;
float tileRadius;
float tileFullHeight;
float tileFullWidth;
hmm_vec2 worldSize;
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

    worldSize.X = tileFullWidth * width;
    worldSize.Y = (tileFullHeight * height) - (tileBottomDisplacement * (height - 1));
    if (height > 1) {
        worldSize.X += tileRadius;
    }

    arrsetlen(WorldArray, width * height);

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            WorldArray[j*width + i].i = j*width + i;
            WorldArray[j*width + i].color.R = rand() / (float)RAND_MAX; // 0.0f;
            WorldArray[j*width + i].color.G = rand() / (float)RAND_MAX; // 0.0f;
            WorldArray[j*width + i].color.B = rand() / (float)RAND_MAX; // 0.0f;
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
    hmm_vec2 startingPosition = {worldSize.X * -0.5f, worldSize.Y * 0.5f};
    startingPosition.X += tileRadius;
    startingPosition.Y += tileFullHeight * -0.5f;
    hmm_vec2 modVector;

    for (int j = 0; j < worldHeight; j++) {
        for (int i = 0; i < worldWidth; i++) {
            modVector.X = tileFullWidth * i;
            modVector.Y = -((tileFullHeight - tileBottomDisplacement) * j);
            if (j % 2) {
                modVector.X += tileRadius;
            }

            glPushMatrix();
                glTranslatef(
                             startingPosition.X + modVector.X,
                             startingPosition.Y + modVector.Y,
                             0.0f
                             );
                glScalef(tileFullHeight, tileFullHeight, 1.0f);
                glColor3fv(WorldArray[j*worldWidth + i].color.Elements);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
            glPopMatrix();
        }
    }
}
