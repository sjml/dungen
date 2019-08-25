#pragma once

void AddChoice(const char* description);
void PresentChoiceSelection(void);
void ClearChoices(void);

void RenderChoices(void);
int GetChoiceStatus(void);
void ProcessMouseMovement(gbVec2 position);
void ProcessMouseClick(bool down);

