#include "../stdafx.h"
#include "game.h"

#include <time.h>

#include "util.h"
#include "world.h"
#include "../scripting/scripting.h"
#include "../hlvm/hlvm.h"
#include "../ui/banner.h"
#include "../ui/choice.h"
#include "../ui/tile_choice.h"

#define MAX_TIMESTEP      1.0
#define TICKS_PER_CYCLE   1
#define SECONDS_PER_CYCLE 0.0

static double previousTime, currentTime;

static unsigned int randomSeed;

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
//    printf("dt: %.4f\n", currentTime - previousTime);
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
        }
    }

    if (waiting) {
        bool done = true;
        if (UpdateBanners((float)dt)) {
            done = false;
        }
        if (GetChoiceStatus() > 0) {
            done = false;
        }
        if (GetTileChoiceStatus() > 0) {
            done = false;
        }
        if (done) {
            SetIntRegister("WaitForUI", 0);
        }
    }
    
    return 0;
}

double GetTime() {
    return glfwGetTime();
}

void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    if (GetChoiceStatus() > 0) {
        gbVec2 pos = {(float)xpos, (float)ypos};
        ChoiceProcessMouseMovement(pos);
    }
    if (GetTileChoiceStatus() > 0) {
        gbVec2 pos = {(float)xpos, (float)ypos};
        TileChoiceProcessMouseMovement(pos);
    }
}

void MouseClickCallback(GLFWwindow* window, int button, int action, int mods) {
    if (GetChoiceStatus() >= 0 && button == GLFW_MOUSE_BUTTON_LEFT) {
        ChoiceProcessMouseClick(action == GLFW_PRESS);
    }
    if (GetTileChoiceStatus() >= 0 && button == GLFW_MOUSE_BUTTON_LEFT) {
        TileChoiceProcessMouseClick(action == GLFW_PRESS);
    }
}
