#include "stdafx.h"
#include "choice.h"

#include "banner.h"
#include "../hlvm/hlvm.h"
#include "../infrastructure/text.h"
#include "../infrastructure/rendering.h"

static int hoveredChoice = -1;
static int pressedChoice = -1;
static char** choices = NULL;

typedef struct {
    gbRect2 bb;
    gbRect2 textBB;
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

    // don't add duplicate choices
    // if (arrlen(choices) > 0) {
    //     char* last = choices[arrlen(choices)-1];
    //     if (strcmp(last, description) == 0) {
    //         return;
    //     }
    // }
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
    choices = NULL;
    arrfree(buttons);
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

    float fontSize = 72.0f;
    float margin = 15.0f;
    float btnW = 1024.0f * 0.48f;
    float btnH =  768.0f * 0.15f;

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
    start.x = (1024.0f * 0.5f) - (totalW * 0.5f) + (btnW * 0.5f);
    start.y = ( 768.0f * 0.5f) - (totalH * 0.5f) + (btnH * 0.5f);
    start.y = 768.0f - start.y;
    Vec2i current = {0, 0};

    bannerHandle = NULL;
    if (strlen(description) > 0) {
        if (numRows <= 5) {
            gbVec4 textColor = {0.3f, 0.3f, 0.7f, 1.0f};
            gbVec4 bgColor = {0.8f, 0.8f, 0.8f, 0.8f};
            bannerHandle = AddBanner(description, 72.0f, textColor, bgColor, -1.0f);
            PositionBanner(bannerHandle, 100.0f);
            start.y -= 50.0f;
        }
    }

    for (long i=0; i < arrlen(choices); i++) {
        gbVec2 center;
        center.x = start.x + (btnW * current.x) + (current.x * margin);
        center.y = start.y - (btnH * current.y) - (current.y * margin);

        Button b;
        b.bb.pos.x = center.x - (btnW * 0.5f);
        b.bb.pos.y = center.y - (btnH * 0.5f);
        b.bb.dim.x = btnW;
        b.bb.dim.y = btnH;

        gbVec2 extents = MeasureTextExtents(choices[i], "fonts/04B_03__.TTF", fontSize);
        b.textBB.pos.x =           center.x - (extents.x * 0.5f);
        b.textBB.pos.y = 768.0f - (center.y - (extents.y * 0.5f));
        b.textBB.dim.x = extents.x;
        b.textBB.dim.y = extents.y;
        arrpush(buttons, b);

        current.y += 1;
        if (current.y >= numRows) {
            current.y = 0;
            current.x += 1;
        }
    }
}

void RenderChoices() {
    for (long i=0; i < arrlen(buttons); i++) {
        if (pressedChoice == i && hoveredChoice == i) {
            glColor4fv(btnColorPress.e);
        }
        else if (hoveredChoice == i) {
            glColor4fv(btnColorHover.e);
        }
        else {
            glColor4fv(btnColorBase.e);
        }

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glBegin(GL_TRIANGLE_STRIP);
            glVertex2f(buttons[i].bb.pos.x + buttons[i].bb.dim.x, buttons[i].bb.pos.y);
            glVertex2f(buttons[i].bb.pos.x + buttons[i].bb.dim.x, buttons[i].bb.pos.y + buttons[i].bb.dim.y);
            glVertex2f(buttons[i].bb.pos.x, buttons[i].bb.pos.y);
            glVertex2f(buttons[i].bb.pos.x, buttons[i].bb.pos.y + buttons[i].bb.dim.y);
        glEnd();
        glPopMatrix();

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        DrawGameText(
            choices[i],
            "fonts/04B_03__.TTF",
            72.0f,
            (int)buttons[i].textBB.pos.x,
            (int)buttons[i].textBB.pos.y,
            0.0f
        );
    }
}

void ChoiceProcessMouseMovement(gbVec2 position) {
    position.y = 768.0f - position.y;
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

        double x, y;
        glfwGetCursorPos(GetWindowHandle(), &x, &y);
        gbVec2 pos = {(float)x, (float)y};
        ChoiceProcessMouseMovement(pos);
    }
}
