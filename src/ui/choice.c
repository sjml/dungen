#include "../stdafx.h"
#include "choice.h"

#include "../hlvm/hlvm.h"
#include "../infrastructure/text.h"

gbVec2 mousePosition;
int hoveredChoice = -1;
char** choices = NULL;

typedef struct {
    gbVec4 boundingBox;
    gbVec2 halfTextExtents;
    gbVec2 position;
} Button;
Button* buttons = NULL;

gbVec4 btnColor = {0.3f, 0.3f, 0.3f, 1.0f};
float btnHalfW = -1.0f;
float btnHalfH = -1.0f;

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
    SetIntRegister("ChoiceSelection", -1);
    for (int i=0; i < arrlen(choices); i++) {
        free(choices[i]);
    }
    arrfree(choices);
    choices = NULL;
    arrfree(buttons);
    buttons = NULL;
}

void PresentChoiceSelection(const char* description) {
    choiceStatus = 1;

    btnHalfW = 1024.0f * 0.4f * 0.5f;
    btnHalfH =  768.0f * 0.15f * 0.5f;

    Button b;
    b.position.x = 1024.0f * 0.5f;
    b.position.y = 768.0f - (768.0f * 0.5f);
    b.boundingBox.x = b.position.x - btnHalfW;
    b.boundingBox.y = b.position.y - btnHalfH;
    b.boundingBox.z = b.position.x + btnHalfW;
    b.boundingBox.w = b.position.y + btnHalfH;
    b.halfTextExtents = MeasureTextExtents("Option A", "fonts/04B_03__.TTF", 72.0f);
    b.halfTextExtents.x *= 0.5f;
    b.halfTextExtents.y *= 0.5f;
    arrpush(buttons, b);

    // if number of choices <= 4, single column, large text (calc size)
    // else if number of choices <= 8, two columns, odd-numbered choices center last, large text (same size)
    // else if number of choices <= 12, two columns, odd-numbered choices center last, medium text (calc size)
    // else throw error
}

void RenderChoices() {
    if (hoveredChoice == 0) {
        glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
    }
    else {
        glColor4fv(btnColor.e);
    }

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(buttons[0].boundingBox.z, buttons[0].boundingBox.y);
        glVertex2f(buttons[0].boundingBox.z, buttons[0].boundingBox.w);
        glVertex2f(buttons[0].boundingBox.x, buttons[0].boundingBox.y);
        glVertex2f(buttons[0].boundingBox.x, buttons[0].boundingBox.w);
    glEnd();
    glPopMatrix();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    DrawGameText("Option A", "fonts/04B_03__.TTF", 72.0f, buttons[0].position.x - buttons[0].halfTextExtents.x, buttons[0].position.y + buttons[0].halfTextExtents.y, 0.0f);
}

void ProcessMouseMovement(gbVec2 position) {
    for (long i=0; i < arrlen(buttons); i++) {

    }

    // see what button we're over, if any
    // if we're over one:
        // if it's not the existing hover index,
            // un-hover that one
            // then update hover index
    // else:
        // if hover index is not -1
            // un-hover that one
            // set hover index to -1
}

void ProcessMouseClick(void) {
    // if hover index is not -1:
        // set ChoiceSelection register
        // set choiceStatus to 0
    // otherwise do nothing
}
