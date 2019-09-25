#include "stdafx.h"
#include "dungen.h"

#include "platform/platform.h"
#include "infrastructure/rendering.h"
#include "scripting/scripting.h"
#include "infrastructure/attributes.h"
#include "infrastructure/game.h"

void InitializeDunGen(const char* startupElement) {
    InitializePlatform();
    InitializeLua();
    InitializeAttributes();
    
    InitializeGame(startupElement);
    InitializeRendering();
}

void RunDunGen(void) {
    bool shouldStop = false;
    while (!shouldStop) {
        bool updateStopping = GameTick();
        bool renderStopping = Render();
        shouldStop = updateStopping || renderStopping;
    }
}

void FinalizeDunGen(void) {
    FinalizeGame();
    
    FinalizeAttributes();
    FinalizeLua();
    FinalizeRendering();
    FinalizePlatform();
}
