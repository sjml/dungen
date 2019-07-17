#include "../stdafx.h"
#include "game.h"

#include <time.h>

#include "util.h"
#include "world.h"
#include "../scripting/scripting.h"
#include "../hlvm/hlvm.h"

double MAX_TIMESTEP = 1.0;
int TICKS_PER_CYCLE = 1;
double SECONDS_PER_CYCLE = 0.0;

double previousTime, currentTime;

#include "./outline.h" // TODO: remove
#include "./rendering.h" // TODO: remove

void InitializeGame(void) {
    srand((unsigned int)time(NULL));
    
    previousTime = glfwGetTime();
    
    if (RunFile("scripts/simulation/WorldSetup.lua") == 0) {
        RunString("push(\"_Root\")");
    }
    
    TileSet ts;
    AddTileToSet(&ts, GetTileAtIndex(0));
    AddTileToSet(&ts, GetTileAtIndex(1));
    AddTileToSet(&ts, GetTileAtPosition(1, 1));
    AddTileToSet(&ts, GetTileAtPosition(3, 2));
    Outline* o = CreateOutline(&ts);
    AddOutline(o);
}

void FinalizeGame(void) {
    FinalizeWorld();
}

double hlvmAccum = 0.0;
int GameTick(void) {
    previousTime = currentTime;
    currentTime = glfwGetTime();
    double dt = gb_clamp(currentTime - previousTime, 0.0, MAX_TIMESTEP);
    
    hlvmAccum += dt;
    if (hlvmAccum >= SECONDS_PER_CYCLE) {
        hlvmAccum = 0.0;
        int hlvmTickCount = 0;
        while (hlvmTickCount++ < TICKS_PER_CYCLE) {
            HLVMProcess();
        }
    }
    
    return 0;
}
