#include "stdafx.h"

#include "infrastructure/rendering.h"
#include "scripting/scripting.h"
#include "infrastructure/attributes.h"
#include "infrastructure/game.h"

int main(int argc, const char * argv[]) {
    
    InitializeRendering();
    InitializeLua();
    InitializeAttributes();

    InitializeGame();
    
    bool shouldStop = 0;
    while (!shouldStop) {
        shouldStop = GameTick();
        shouldStop = Render();
    }
    
    FinalizeGame();
    
    FinalizeAttributes();
    FinalizeLua();
    FinalizeRendering();
    
    return EXIT_SUCCESS;

}
