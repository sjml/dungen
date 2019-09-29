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

void _TickAndRender() {
    GameTick();
    Render();
}

void RunDunGen(void) {
    #if !(DUNGEN_WASM)
        bool shouldStop = false;
        while (!shouldStop) {
            bool updateStopping = GameTick();
            bool renderStopping = Render();
            shouldStop = updateStopping || renderStopping;
        }
    #else
        emscripten_set_main_loop(_TickAndRender, 0, 0);
    #endif // DUNGEN_WASM
}

void FinalizeDunGen(void) {
    FinalizeGame();

    FinalizeAttributes();
    FinalizeLua();
    FinalizeRendering();
    FinalizePlatform();
}
