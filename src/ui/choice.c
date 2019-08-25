#include "../stdafx.h"
#include "choice.h"

#include "../hlvm/hlvm.h"
#include "../infrastructure/text.h"
#include "../infrastructure/rendering.h"

int hoveredChoice = -1;
int pressedChoice = -1;
char** choices = NULL;

typedef struct {
    gbRect2 bb;
    gbRect2 textBB;
} Button;
Button* buttons = NULL;

gbVec4 btnColorBase  = {0.3f, 0.3f, 0.3f, 1.0f};
gbVec4 btnColorHover = {0.4f, 0.4f, 0.4f, 1.0f};
gbVec4 btnColorPress = {0.6f, 0.6f, 0.6f, 1.0f};

int choiceStatus = -1; // -1 --> no choice is being presented right now
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

    if (arrlen(choices) > 0) {
        char* last = choices[arrlen(choices)-1];
        if (strcmp(last, description) == 0) {
            return; // don't add duplicate choices
        }
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
    choices = NULL;
    arrfree(buttons);
    buttons = NULL;
}

void PresentChoiceSelection(const char* description) {
    if (arrlen(choices) == 0) {
        fprintf(stderr, "WARNING: Cannot present empty choice selection.\n");
        return;
    }

    choiceStatus = 1;

    float fontSize = 72.0f;
    float margin = 15.0f;
    float btnHalfW = 1024.0f * 0.4f * 0.5f;
    float btnHalfH =  768.0f * 0.15f * 0.5f;
    
    gbVec2 middle;
    middle.x = 1024.0f * 0.5f;
    middle.y = 768.0f - (768.0f * 0.5f);
    float totalHeight = ((btnHalfH * 2.0f) * (arrlen(choices))) + ((arrlen(choices)-1) * margin);
    
    gbVec2 current;
    current.x = middle.x;
    current.y = middle.y + ((totalHeight * 0.5f) - btnHalfH);
    
    for (long i=0; i < arrlen(choices); i++) {
        Button b;
        b.bb.pos.x = current.x - btnHalfW;
        b.bb.pos.y = current.y - btnHalfH;
        b.bb.dim.x = btnHalfW * 2.0f;
        b.bb.dim.y = btnHalfH * 2.0f;

        gbVec2 extents = MeasureTextExtents(choices[i], "fonts/04B_03__.TTF", fontSize);
        b.textBB.pos.x =           current.x - (extents.x * 0.5f);
        b.textBB.pos.y = 768.0f - (current.y - (extents.y * 0.5f));
        b.textBB.dim.x = extents.x;
        b.textBB.dim.y = extents.y;
        arrpush(buttons, b);
        
        current.y -= (btnHalfH * 2.0f) + margin;
    }


    // if number of choices <= 4, single column, large text (calc size)
    // else if number of choices <= 8, two columns, odd-numbered choices center last, large text (same size)
    // else if number of choices <= 12, two columns, odd-numbered choices center last, medium text (calc size)
    // else throw error
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
            buttons[i].textBB.pos.x,
            buttons[i].textBB.pos.y,
            0.0f
        );
    }
}

void ProcessMouseMovement(gbVec2 position) {
    position.y = 768.0f - position.y;
    for (long i=0; i < arrlen(buttons); i++) {
        if (gb_rect2_contains_vec2(buttons[i].bb, position)) {
            hoveredChoice = (int)i;
            return;
        }
    }
    hoveredChoice = -1;
}

void ProcessMouseClick(bool down) {
    if (down) {
        if (hoveredChoice != -1) {
            pressedChoice = hoveredChoice;
        }
    }
    else {
        if (pressedChoice == hoveredChoice) {
            // make the choice
            SetIntRegister("ChoiceSelection", pressedChoice);
            choiceStatus = 0;
        }
        pressedChoice = -1;

        double x, y;
        glfwGetCursorPos(GetWindowHandle(), &x, &y);
        gbVec2 pos = {x, y};
        ProcessMouseMovement(pos);
    }
}
