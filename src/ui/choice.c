#include "../stdafx.h"
#include "choice.h"

#include "../hlvm/hlvm.h"

gbVec2 mousePosition;
int hoveredChoice = -1;
char** choices = NULL;

int choiceStatus = -1; // -1 --> no choice is being presented right now
                       //  1 --> a choice is currently being presented, no option has been chosen
                       //  0 --> a choice is currently being presented, and an option has been chosen

int GetChoiceStatus(void) {
    return choiceStatus;
}

void AddChoice(const char* description) {
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
}

void PresentChoiceSelection(const char* description) {
    choiceStatus = 1;
    
    // if number of choices <= 4, single column, large text (calc size)
    // else if number of choices <= 8, two columns, odd-numbered choices center last, large text (same size)
    // else if number of choices <= 12, two columns, odd-numbered choices center last, medium text (calc size)
    // else throw error
}

void ProcessMouseMovement(gbVec2 position) {
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
