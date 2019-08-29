#pragma once

void AddChoice(const char* description);
void PresentChoiceSelection(const char* description);
void ClearChoices(void);

void RenderChoices(void);
int GetChoiceStatus(void);
void ChoiceProcessMouseMovement(gbVec2 position);
void ChoiceProcessMouseClick(bool down);

