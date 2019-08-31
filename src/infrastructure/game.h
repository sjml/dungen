#pragma once

void InitializeGame(const char* startupElement);
void FinalizeGame(void);

int GameTick(void);

double GetTime(void);

void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);
void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
