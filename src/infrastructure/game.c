#include "../stdafx.h"
#include "game.h"

#include <time.h>

#include "util.h"
#include "../scripting/scripting.h"
#include "../hlvm/hlvm.h"

double MAX_TIMESTEP = 1.0;

double previousTime, currentTime;

void InitializeGame(void) {
    srand((unsigned int)time(NULL));
    
    previousTime = glfwGetTime();
    
    RunFile("scripts/simulation/WorldSetup.lua");
}

void FinalizeGame(void) {
    
}

int GameTick(void) {
//    previousTime = currentTime;
//    currentTime = glfwGetTime();
//    double dt = gb_clamp(currentTime - previousTime, 0.0, MAX_TIMESTEP);
    
    HLVMProcess();
    
    return 0;
}
