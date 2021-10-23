#include "stdafx.h"
#include "tools.h"

#if !defined(DUNGEN_DISABLE_TOOLS)
    #define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
    #include <cimgui.h>
#endif // defined(DUNGEN_DISABLE_TOOLS)

#include "rendering.h"
#include "attributes.h"
#include "world.h"
#include "../scripting/scripting.h"

#if !defined(DUNGEN_DISABLE_TOOLS)
    static bool toolsVisible = false;
#endif // !defined(DUNGEN_DISABLE_TOOLS)

void ShowTools(bool show) {
    #if !defined(DUNGEN_DISABLE_TOOLS)
        toolsVisible = show;
    #else
        (void)show; // unused variable
    #endif // defined(DUNGEN_DISABLE_TOOLS)
}

bool AreToolsVisible(void) {
    #if defined(DUNGEN_DISABLE_TOOLS)
        return false;
    #else
        return toolsVisible;
    #endif // defined(DUNGEN_DISABLE_TOOLS)
}

#if defined(DUNGEN_DISABLE_TOOLS)
    void RenderTools(void) {
        return;
    }
#else
    void RenderTools(void) {

        if (!toolsVisible) {
            return;
        }

        igSetNextWindowPos((ImVec2){10,10}, ImGuiCond_Once, (ImVec2){0,0});
        igBegin("DunGen Tools", 0,
            ImGuiWindowFlags_AlwaysAutoResize
        );

        if (igButton("Reset World", (ImVec2){150, 20})) {
            FinalizeAttributes();
            FinalizeWorld();

            InitializeAttributes();
            RunFile("scripts/simulation/WorldSetup.lua");
        }

        if (igTreeNode_Str("Lua Files")) {
            igCheckbox("Checkbox?", &toolsVisible);
            igTreePop();
            igSeparator();
        }

        igEnd();
    }
#endif // defined(DUNGEN_DISABLE_TOOLS)
