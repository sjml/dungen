#include "stdafx.h"
#include "dungen.h"

#include "infrastructure/attributes.h"
#include "infrastructure/game.h"
#include "infrastructure/rendering.h"
#include "infrastructure/tools.h"
#include "platform/platform.h"
#include "scripting/scripting.h"

void InitializeDunGen(const char* startupElement) {
    InitializePlatform();

    #if !defined(DUNGEN_DISABLE_TOOLS)
        InitializeTools();
    #endif

    InitializeLua();
    InitializeAttributes();
    InitializeGame(startupElement);
    InitializeRendering();
}

void TickAndRender() {
    GameTick();
    Render();
}

void EventCallback(const sapp_event* event) {
    ProcessEvent(event);
}

void FinalizeDunGen(void) {
    FinalizeGame();

    FinalizeAttributes();
    FinalizeLua();
    FinalizeRendering();

    #if !defined(DUNGEN_DISABLE_TOOLS)
        FinalizeTools();
    #endif

    FinalizePlatform();
}
