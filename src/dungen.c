#include "stdafx.h"
#include "dungen.h"

#include "platform/platform.h"
#include "infrastructure/rendering.h"
#include "scripting/scripting.h"
#include "infrastructure/attributes.h"
#include "infrastructure/game.h"

void InitializeDunGen(const char* startupElement) {
    InitializePlatform();
    InitializeRendering();
    InitializeLua();
    InitializeAttributes();
    InitializeGame(startupElement);
}

bool _TickAndRender() {
    bool updateStopping = GameTick();
    bool renderStopping = Render();
    return updateStopping || renderStopping;
}

void _RawTickAndRender() {
    GameTick();
    Render();
}

void RunDunGen(void) {
    #if !(DUNGEN_WASM)
        bool shouldStop = false;
        while (!shouldStop) {
            shouldStop = _TickAndRender();
        }
    #else
        emscripten_set_main_loop(_RawTickAndRender, 0, 0);
    #endif // DUNGEN_WASM
}

void FinalizeDunGen(void) {
    FinalizeGame();

    FinalizeAttributes();
    FinalizeLua();
    FinalizeRendering();
    FinalizePlatform();
}
