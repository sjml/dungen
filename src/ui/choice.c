#include "stdafx.h"
#include "choice.h"

#include "banner.h"
#include "../hlvm/hlvm.h"
#include "../infrastructure/text.h"
#include "../infrastructure/rendering.h"
#include "../infrastructure/game.h"

static int hoveredChoice = -1;
static int pressedChoice = -1;
static char** choices = NULL;
static TextInfo** choiceTexts = NULL;

typedef struct {
    gbRect2 bb;
    gbRect2 textBB;
    sg_buffer vertBuff;
} Button;
static Button* buttons = NULL;

static void* bannerHandle = NULL;

static gbVec4 btnColorBase  = {0.3f, 0.3f, 0.3f, 1.0f};
static gbVec4 btnColorHover = {0.4f, 0.4f, 0.4f, 1.0f};
static gbVec4 btnColorPress = {0.6f, 0.6f, 0.6f, 1.0f};

static int choiceStatus = -1; // -1 --> no choice is being presented right now
                              //  1 --> a choice is currently being presented, no option has been chosen
                              //  0 --> a choice is currently being presented, and an option has been chosen
int GetChoiceStatus(void) {
    return choiceStatus;
}

void AddChoice(const char* description) {
    if (arrlen(choices) >= 12) {
        fprintf(stderr, "ERROR: Cannot have more than 12 choices. Ignoring '%s'.\n", description);
        return;
    }

    char* c = malloc(sizeof(char) * strlen(description) + 1);
    strcpy(c, description);
    arrpush(choices, c);
}

void ClearChoices(void) {
    choiceStatus = -1;
    for (int i=0; i < arrlen(choices); i++) {
        free(choices[i]);
    }
    arrfree(choices);

    for (int i=0; i < arrlen(buttons); i++) {
        sg_destroy_buffer(buttons[i].vertBuff);
    }
    arrfree(buttons);

    for (int i=0; i < arrlen(choiceTexts); i++) {
        DestroyTextInfo(choiceTexts[i]);
    }
    arrfree(choiceTexts);

    buttons = NULL;
}

void PresentChoiceSelection(const char* description) {
    int numChoices = (int)arrlen(choices);

    if (numChoices == 0) {
        fprintf(stderr, "WARNING: Cannot present empty choice selection.\n");
        return;
    }
    else if (numChoices > 12) {
        fprintf(stderr, "WARNING: Cannot present overfull choice selection.\n");
        return;
    }

    choiceStatus = 1;

    Vec2i dims = GetOrthoDimensions();

    float fontScale = 2.25f;
    float margin = 15.0f;
    float btnW = dims.x * 0.48f;
    float btnH = dims.y * 0.15f;

    int numRows = 0, numCols = 0;
    if (numChoices <= 4) {
        numCols = 1;
        numRows = (int)arrlen(choices);
        btnW += 200.0f;
    }
    else if (numChoices <= 8) {
        numCols = 2;
        if (numChoices <= 6) {
            numRows = 3;
        }
        else {
            numRows = 4;
        }
    }
    else if (numChoices <= 12) {
        numCols = 2;
        if (numChoices <= 10) {
            numRows = 5;
        }
        else {
            numRows = 6;
        }
    }

    float totalH = (btnH * numRows) + ((numRows-1) * margin);
    float totalW = (btnW * numCols) + ((numCols-1) * margin);

    gbVec2 start;
    start.x = (dims.x * 0.5f) - (totalW * 0.5f) + (btnW * 0.5f);
    start.y = (dims.y * 0.5f) - (totalH * 0.5f) + (btnH * 0.5f);
    Vec2i current = {0, 0};

    bannerHandle = NULL;
    if (strlen(description) > 0) {
        if (numRows <= 5) {
            gbVec4 textColor = {0.3f, 0.3f, 0.7f, 1.0f};
            gbVec4 bgColor = {0.8f, 0.8f, 0.8f, 0.8f};
            bannerHandle = AddBanner(description, 2.0f, textColor, bgColor, -1.0f);
            PositionBanner(bannerHandle, 100.0f);
            start.y += 50.0f;
        }
    }

    for (long i=0; i < arrlen(choices); i++) {
        gbVec2 center;
        center.x = start.x + (btnW * current.x) + (current.x * margin);
        center.y = start.y + (btnH * current.y) + (current.y * margin);

        Button b;
        b.bb.pos.x = center.x - (btnW * 0.5f);
        b.bb.pos.y = center.y - (btnH * 0.5f);
        b.bb.dim.x = btnW;
        b.bb.dim.y = btnH;

        gbVec2 extents = MeasureTextExtents(choices[i], "Pixel", fontScale);
        b.textBB.pos.x = center.x - (extents.x * 0.5f);
        b.textBB.pos.y = center.y + (extents.y * 0.5f);
        b.textBB.dim.x = extents.x;
        b.textBB.dim.y = extents.y;

        float x0, y0, x1, y1;
        x0 = b.bb.pos.x;
        y1 = b.bb.pos.y;
        x1 = b.bb.pos.x + b.bb.dim.x;
        y0 = b.bb.pos.y + b.bb.dim.y;

        float verts[8];
        verts[0] = x0;
        verts[1] = y0;
        verts[2] = x1;
        verts[3] = y0;
        verts[4] = x0;
        verts[5] = y1;
        verts[6] = x1;
        verts[7] = y1;
        b.vertBuff = sg_make_buffer(&(sg_buffer_desc){
            .content = verts,
            .size = sizeof(verts)
        });

        arrpush(buttons, b);

        current.y += 1;
        if (current.y >= numRows) {
            current.y = 0;
            current.x += 1;
        }

        arrpush(choiceTexts, CreateTextInfo(choices[i], "Pixel", b.textBB.pos, 2.25f, (gbVec4){1.0f, 1.0f, 1.0f, 1.0f}));
    }

    ChoiceProcessMouseMovement(GetCursorPosition());
}

void RenderChoices(gbMat4* matrix) {
    if (arrlen(buttons) == 0) {
        return;
    }

    for (long i=0; i < arrlen(buttons); i++) {
        if (pressedChoice == i && hoveredChoice == i) {
            DrawShapeBuffer(buttons[i].vertBuff, 4, btnColorPress, matrix);
        }
        else if (hoveredChoice == i) {
            DrawShapeBuffer(buttons[i].vertBuff, 4, btnColorHover, matrix);
        }
        else {
            DrawShapeBuffer(buttons[i].vertBuff, 4, btnColorBase, matrix);
        }
    }

   PrepDrawText(matrix);
   for (long i=0; i < arrlen(buttons); i++) {
       DrawText(choiceTexts[i]);
   }
   FinishDrawText();
}

void ChoiceProcessMouseMovement(gbVec2 position) {
    for (long i=0; i < arrlen(buttons); i++) {
        if (gb_rect2_contains_vec2(buttons[i].bb, position)) {
            hoveredChoice = (int)i;
            return;
        }
    }
    hoveredChoice = -1;
}

void ChoiceProcessMouseClick(bool down) {
    if (down) {
        if (hoveredChoice != -1) {
            pressedChoice = hoveredChoice;
        }
    }
    else {
        if (pressedChoice != -1 && (pressedChoice == hoveredChoice)) {
            // make the choice
            SetIntRegister("ChoiceSelection", pressedChoice);
            choiceStatus = 0;
            if (bannerHandle != NULL) {
                RemoveBanner(bannerHandle);
                bannerHandle = NULL;
            }
        }
        pressedChoice = -1;

        ChoiceProcessMouseMovement(GetCursorPosition());
    }
}
