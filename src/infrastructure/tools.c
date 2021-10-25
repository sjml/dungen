#include "stdafx.h"
#include "tools.h"

#if !defined(DUNGEN_DISABLE_TOOLS)
    #define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
    #include <cimgui.h>
#endif // defined(DUNGEN_DISABLE_TOOLS)

#include "rendering.h"
#include "attributes.h"
#include "files.h"
#include "world.h"
#include "../hlvm/hlvm.h"
#include "../scripting/scripting.h"


static bool toolsVisible = false;
static FileInfo* scriptListing = NULL;

void InitializeTools(void) {
    if (scriptListing != NULL) {
        FileInfoFree(scriptListing);
    }
    scriptListing = GetFileSystemInformation("scripts/simulation/elements");
}

void FinalizeTools(void) {
    if (scriptListing != NULL) {
        FileInfoFree(scriptListing);
    }
}

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
    void RenderSimFileTree(FileInfo* fi, int strip) {
        int baseLen = (int)strlen(fi->path);
        if (fi->isDirectory) {
            char* label = fi->path + (sizeof(char) * strip);
            if (baseLen == strip) {
                label = "Simulation Elements";
            }
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
            if (label[0] != '_') {
                flags |= ImGuiTreeNodeFlags_DefaultOpen;
            }
            if (igTreeNodeEx_Str(label, flags)) {
                for (int i=0; i < arrlen(fi->children); i++) {
                    RenderSimFileTree(fi->children[i], baseLen);
                }
                igTreePop();
            }
        }
        else {
            igPushID_Str(fi->path);
            if (igSmallButton("push")) {
                size_t rootLen = strlen(scriptListing->path);
                size_t elPathLen = strlen(fi->path) - rootLen - 4 + 1; // minus ".lua" + the null terminator
                char* elPath = malloc(sizeof(char) * elPathLen);
                size_t idx = 0;
                char c;
                do {
                    c = fi->path[idx+rootLen];
                    if (c == '/') {
                        elPath[idx] = '.';
                    }
                    else {
                        elPath[idx] = c;
                    }
                    idx++;
                } while (c != '\0' && idx < elPathLen);
                elPath[elPathLen-1] = '\0';

                PushSimulationElement(elPath);

                free(elPath);
            }
            igPopID();
            igSameLine(0.0f, -1.0f);
            igText(fi->path + (sizeof(char) * strip));
        }
    }

    void RenderTools(void) {
        if (!toolsVisible) {
            return;
        }

        igSetNextWindowPos((ImVec2){10,10}, ImGuiCond_Once, (ImVec2){0,0});
        igBegin("DunGen Tools", 0,
              ImGuiWindowFlags_AlwaysAutoResize
        );

        igBeginGroup();
        if (igButton("Reset World", (ImVec2){150, 20})) {
            FinalizeWorld();
            FinalizeAttributes();

            InitializeHLVM();
            InitializeAttributes();
            RunFile("scripts/simulation/WorldSetup.lua");
        }
        if (igButton("Reload Script Files", (ImVec2){150, 20})) {
            if (scriptListing != NULL) {
                FileInfoFree(scriptListing);
            }
            scriptListing = GetFileSystemInformation("scripts/simulation/elements");

            RunString("loadFiles(\"scripts/simulation/elements\", \"\")");
        }

        if (scriptListing != NULL) {
            RenderSimFileTree(scriptListing, (int)strlen(scriptListing->path));
        }
        igEndGroup();

        igSameLine(0.0f, -1.0f);

        igBeginGroup();

        if (igCollapsingHeader_BoolPtr("Int Registers", 0, ImGuiTreeNodeFlags_DefaultOpen)) {
            char** regNames = ListIntRegisters();
            for (int i=0; i < arrlen(regNames); i++) {
                int val = GetIntRegister(regNames[i]);
                if (igInputInt(regNames[i], &val, 1, 10, ImGuiInputTextFlags_None)) {
                    SetIntRegister(regNames[i], val);
                }
            }
            arrfree(regNames);
        }

        igEndGroup();


        igEnd();
    }
#endif // defined(DUNGEN_DISABLE_TOOLS)
