#include "stdafx.h"

#include "infrastructure/rendering.h"
#include "infrastructure/game.h"
#include "scripting/scripting.h"

#include "world.h"

int main(int argc, const char * argv[]) {
    
    InitializeRendering();
    InitializeLua();

    InitializeGame();
    
    bool shouldStop = 0;
    while (!shouldStop) {
        shouldStop = GameTick();
        shouldStop = Render();
    }
    
    FinalizeLua();
    FinalizeRendering();
    
    return EXIT_SUCCESS;
}
