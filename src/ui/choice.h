#pragma once

void AddChoice(const char* description);
void PresentChoiceSelection(void);
void ClearChoices(void);

void RenderChoices(void);
int GetChoiceStatus(void);
void ChoiceProcessMouseMovement(gbVec2 position);
void ChoiceProcessMouseClick(bool down);

