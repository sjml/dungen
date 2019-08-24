#include "../stdafx.h"
#include "game.h"

#include <time.h>

#include "util.h"
#include "world.h"
#include "../scripting/scripting.h"
#include "../hlvm/hlvm.h"
#include "../ui/banner.h"

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

    if (!UpdateBanners((float)dt) && waiting) {
        SetIntRegister("WaitForUI", 0);
    }

    return 0;
}

double GetTime() {
    return glfwGetTime();
}

void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
}
