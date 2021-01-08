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
    FinalizePlatform();
}
