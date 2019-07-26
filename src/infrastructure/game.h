#pragma once

void InitializeGame(void);
void FinalizeGame(void);

int GameTick(void);

double GetTime(void);

void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
