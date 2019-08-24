#include "../stdafx.h"
#include "game.h"

#include <time.h>

#include "util.h"
#include "world.h"
#include "../scripting/scripting.h"
#include "../hlvm/hlvm.h"
#include "../ui/banner.h"
#include "../ui/choice.h"

#define MAX_TIMESTEP      1.0
#define TICKS_PER_CYCLE   1
#define SECONDS_PER_CYCLE 0.0

double previousTime, currentTime;

unsigned int randomSeed;

void InitializeGame(void) {
    randomSeed = (unsigned int)time(NULL);
    srand(randomSeed);

    previousTime = glfwGetTime();

    InitializeHLVM();

    if (RunFile("scripts/simulation/WorldSetup.lua") == 0) {
        RunString("push(\"_Root\")");
    }
    
    AddChoice("Option A");
    AddChoice("Option A");
    AddChoice("Option B");
    
    PresentChoiceSelection("Here is a choice.");
}

void FinalizeGame(void) {
    FinalizeWorld();
}

double hlvmAccum = 0.0;
int GameTick(void) {
    previousTime = currentTime;
    currentTime = glfwGetTime();
    double dt = gb_clamp(currentTime - previousTime, 0.0, MAX_TIMESTEP);

    bool waiting = GetIntRegister("WaitForUI") != 0;
    if (!waiting) {
        hlvmAccum += dt;
        if (hlvmAccum >= SECONDS_PER_CYCLE) {
            hlvmAccum = 0.0;
            int hlvmTickCount = 0;
            while (hlvmTickCount++ < TICKS_PER_CYCLE) {
                RunString("HLVMProcess()");
            }
            RunString("ResolveStyles()");
        }
    }

    if (!UpdateBanners((float)dt) && waiting && GetChoiceStatus() <= 0) {
        SetIntRegister("WaitForUI", 0);
    }

    return 0;
}

double GetTime() {
    return glfwGetTime();
}

void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    if (GetChoiceStatus() >= 0) {
        gbVec2 pos = {(float)xpos, (float)ypos};
        ProcessMouseMovement(pos);
    }
}

void MouseClickCallback(GLFWwindow* window, int button, int action, int mods) {
    if (GetChoiceStatus() >= 0 && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        ProcessMouseClick();
    }
}
