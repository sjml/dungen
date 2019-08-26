#include "../stdafx.h"
#include "tile_choice.h"

#include "../infrastructure/world.h"
#include "../infrastructure/rendering.h"
#include "../hlvm/hlvm.h"

static Region* hoveredSingle = NULL;
static TileData* hoveredTile = NULL;
static TileData* pressedTile = NULL;

static TileNEWSet* valid;

static int choiceStatus = -1; // -1 --> no choice is being presented right now
                              //  1 --> a choice is currently being presented, no option has been chosen
                              //  0 --> a choice is currently being presented, and an option has been chosen
int GetTileChoiceStatus(void) {
    return choiceStatus;
}

void PresentTileChoice(void) {
    valid = GetTileSetRegister("TileChoiceValidSet");
    if (GetTileSetCount(valid) == 0) {
        valid = NULL;
        fprintf(stderr, "WARNING: Cannot present empty tile choice selection.\n");
        return;
    }

    choiceStatus = 1;
    Vec2i dims = GetWorldDimensions();

    hoveredSingle = CreateRegion();

    for (long i = 0; i < dims.x * dims.y; i++) {
        TileData* t = GetTileAtIndex(i);
        if (!IsTileInSet(valid, t)) {
            t->overlayColor.r = 1.0f;
            t->overlayColor.g = 1.0f;
            t->overlayColor.b = 1.0f;
            t->overlayColor.a = 0.5f;
        }
    }
}

void RenderTileChoice(void) {

}

void TileChoiceProcessMouseMovement(gbVec2 position) {
    TileData* current = ScreenToTile(&position);
    if (current == hoveredTile) {
        return;
    }

    if (hoveredTile != NULL && hoveredTile == pressedTile) {
        pressedTile->overlayColor.r = 0.0f;
        pressedTile->overlayColor.g = 0.0f;
        pressedTile->overlayColor.b = 0.0f;
        pressedTile->overlayColor.a = 0.0f;
    }
    if (current != NULL && current == pressedTile) {
        pressedTile->overlayColor.r = 1.0f;
        pressedTile->overlayColor.g = 0.0f;
        pressedTile->overlayColor.b = 0.0f;
        pressedTile->overlayColor.a = 1.0f;
    }

    if (hoveredTile != NULL) {
        RemoveTileFromRegion(hoveredSingle, hoveredTile);
    }
    if (current != NULL && IsTileInSet(valid, current)) {
        AddTileToRegion(hoveredSingle, current);
    }
    ClearRegionOutline(hoveredSingle);
    gbVec4 hoverColor = {1.0f, 0.0f, 0.0f, 1.0f};
    SetRegionOutline(hoveredSingle, hoverColor, 0.25f);

    hoveredTile = current;
}

void TileChoiceProcessMouseClick(bool down) {
    if (down) {
        if (hoveredTile != NULL && IsTileInSet(valid, hoveredTile)) {
            pressedTile = hoveredTile;
            pressedTile->overlayColor.r = 1.0f;
            pressedTile->overlayColor.g = 0.0f;
            pressedTile->overlayColor.b = 0.0f;
            pressedTile->overlayColor.a = 1.0f;
        }
    }
    else {
        if (pressedTile == hoveredTile) {
            // make the choice
            choiceStatus = 0;
            SetTileRegister("TileChoiceSelection", pressedTile);
            DestroyRegion(hoveredSingle);
            hoveredSingle = NULL;
            hoveredTile = NULL;

            Vec2i dims = GetWorldDimensions();
            for (long i = 0; i < dims.x * dims.y; i++) {
                TileData* t = GetTileAtIndex(i);
                t->overlayColor.r = 0.0f;
                t->overlayColor.g = 0.0f;
                t->overlayColor.b = 0.0f;
                t->overlayColor.a = 0.0f;
            }
        }
        pressedTile->overlayColor.r = 0.0f;
        pressedTile->overlayColor.g = 0.0f;
        pressedTile->overlayColor.b = 0.0f;
        pressedTile->overlayColor.a = 0.0f;
        pressedTile = NULL;

        if (choiceStatus > 0) {
            double x, y;
            glfwGetCursorPos(GetWindowHandle(), &x, &y);
            gbVec2 pos = {(float)x, (float)y};
            TileChoiceProcessMouseMovement(pos);
        }
    }
}
