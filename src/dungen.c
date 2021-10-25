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
    InitializeLua();
    InitializeAttributes();
    InitializeGame(startupElement);
    InitializeRendering();

    #if !defined(DUNGEN_DISABLE_TOOLS)
        InitializeTools();
    #endif
}

void TickAndRender() {
    GameTick();
    Render();
}

void EventCallback(const sapp_event* event) {
    ProcessEvent(event);
}

void FinalizeDunGen(void) {
    #if !defined(DUNGEN_DISABLE_TOOLS)
        FinalizeTools();
    #endif

    FinalizeGame();

    FinalizeAttributes();
    FinalizeLua();
    FinalizeRendering();
    FinalizePlatform();
}
