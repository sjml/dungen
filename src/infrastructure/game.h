#pragma once

void InitializeGame(void);
void FinalizeGame(void);

int GameTick(void);

double GetTime(void);

void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);
