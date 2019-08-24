#pragma once

void AddChoice(const char* description);
void PresentChoiceSelection(const char* description);
void ClearChoices(void);

int GetChoiceStatus(void);
void ProcessMouseMovement(gbVec2 position);
void ProcessMouseClick(void);
