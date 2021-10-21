#include "stdafx.h"
#include "console.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

static bool consoleShowing = false;

void toggleConsole(void) {
    consoleShowing = !consoleShowing;
}

void drawConsole(void) {
    if (!consoleShowing) {
        return;
    }
    
    igSetNextWindowPos((ImVec2){10,10}, ImGuiCond_Once, (ImVec2){0,0});
    igSetNextWindowSize((ImVec2){400, 100}, ImGuiCond_Once);
    igBegin("Lua Console", 0, ImGuiWindowFlags_None);
    igEnd();
}