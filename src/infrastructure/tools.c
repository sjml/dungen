#include "stdafx.h"
#include "tools.h"

#if !defined(DUNGEN_DISABLE_TOOLS)
    #define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
    #include <cimgui.h>
#endif // defined(DUNGEN_DISABLE_TOOLS)

#include "rendering.h"

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

        Vec2i fbSizei = GetFramebufferDimensions();
        gbVec2 fbSize = { (float)fbSizei.x, (float)fbSizei.y };
        fbSize.x = fbSize.x * 0.75f;
        fbSize.y = fbSize.y * 0.75f;

        igSetNextWindowPos((ImVec2){10,10}, ImGuiCond_Once, (ImVec2){0,0});
        igSetNextWindowSize((ImVec2){fbSize.x, fbSize.y}, ImGuiCond_Once);
        igBegin("DunGen Tools", 0, ImGuiWindowFlags_None);
        igEnd();
    }
#endif // defined(DUNGEN_DISABLE_TOOLS)
