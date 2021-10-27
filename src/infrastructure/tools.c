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
static char* inputBuffer = NULL;
static int inputBufferSize = 1024;
static bool consoleFocusRequestActive = false;

void InitializeTools(void) {
    if (scriptListing != NULL) {
        FileInfoFree(scriptListing);
    }
    scriptListing = GetFileSystemInformation("scripts/simulation/elements");

    inputBuffer = malloc(inputBufferSize);
    inputBuffer[0] = '\0';
}

void FinalizeTools(void) {
    if (scriptListing != NULL) {
        FileInfoFree(scriptListing);
    }

    free(inputBuffer);
}

void ShowTools(bool show) {
    toolsVisible = show;
}

bool AreToolsVisible(void) {
    #if defined(DUNGEN_DISABLE_TOOLS)
        return false;
    #else
        return toolsVisible;
    #endif // defined(DUNGEN_DISABLE_TOOLS)
}

void RequestConsoleFocus(void) {
    consoleFocusRequestActive = true;
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
        igSetNextWindowSizeConstraints((ImVec2){1004, 500}, (ImVec2){1004, 748}, NULL, NULL);
        igBegin("DunGen Tools", 0, ImGuiNextWindowDataFlags_None
            // | ImGuiWindowFlags_AlwaysAutoResize
        );

        igBeginGroup();
        igBeginChild_Str("Simulation Control", (ImVec2){300, -1}, false, ImGuiWindowFlags_AlwaysAutoResize);
        if (igButton("Reset World", (ImVec2){135, 20})) {
            FinalizeWorld();
            FinalizeAttributes();

            InitializeHLVM();
            InitializeAttributes();
            RunFile("scripts/simulation/WorldSetup.lua");
        }
        igSameLine(0.0f, -1.0f);
        if (igButton("Reload Scripts", (ImVec2){135, 20})) {
            if (scriptListing != NULL) {
                FileInfoFree(scriptListing);
            }
            scriptListing = GetFileSystemInformation("scripts/simulation/elements");

            RunString("loadFiles(\"scripts/simulation/elements\", \"\")");
        }

        if (scriptListing != NULL) {
            RenderSimFileTree(scriptListing, (int)strlen(scriptListing->path));
        }
        igEndChild();
        igEndGroup();

        igSameLine(0.0f, -1.0f);

        igBeginGroup();
        igBeginChild_Str("Registers Control", (ImVec2){300, -1}, false, ImGuiWindowFlags_AlwaysAutoResize);
        igPushItemWidth(150);
        if (igCollapsingHeader_BoolPtr("Int Registers", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
            char** regNames = ListIntRegisters();
            for (int i=0; i < arrlen(regNames); i++) {
                int val = GetIntRegister(regNames[i]);
                if (igInputInt(regNames[i], &val, 1, 10, ImGuiInputTextFlags_None)) {
                    SetIntRegister(regNames[i], val);
                }
            }
            arrfree(regNames);
        }

        if (igCollapsingHeader_BoolPtr("Float Registers", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
            char** regNames = ListFloatRegisters();
            for (int i=0; i < arrlen(regNames); i++) {
                float val = GetFloatRegister(regNames[i]);
                if (igInputFloat(regNames[i], &val, 0.1f, 1.0f, NULL, ImGuiInputTextFlags_None)) {
                    SetFloatRegister(regNames[i], val);
                }
            }
            arrfree(regNames);
        }

        if (igCollapsingHeader_BoolPtr("String Registers", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
            char** regNames = ListStringRegisters();
            for (int i=0; i < arrlen(regNames); i++) {
                igText("%s: %s", regNames[i], GetStringRegister(regNames[i]));
            }
            arrfree(regNames);
        }
        igPopItemWidth();
        igEndChild();
        igEndGroup();

        igSameLine(0.0f, -1.0f);

        igBeginGroup();
        ImVec2 avail;
        igGetContentRegionAvail(&avail);
        igBeginChild_Str("Lua Console", (ImVec2){0, 0}, false, ImGuiWindowFlags_AlwaysAutoResize);
        igBeginChild_Str("Lua Printout", (ImVec2){0, avail.y - 25}, true, ImGuiWindowFlags_None);
        igTextWrapped("Here is where the Lua console will be."
            "\n\n"
            "Veniam do consequat proident ex ea cupidatat eiusmod ea cillum duis id mollit et. Qui non nisi culpa deserunt non reprehenderit occaecat cupidatat velit dolore velit eu. Fugiat minim adipisicing aliqua dolor adipisicing velit ea tempor anim nostrud ut."
            "\n\n"
            "Amet excepteur cupidatat labore non labore elit occaecat pariatur tempor veniam. Tempor reprehenderit enim quis veniam culpa aute. Ad sit aute tempor tempor do ut duis tempor aute ea. Non exercitation dolore est cillum ut laborum."
            "\n\n"
            "Dolore exercitation in aliquip Lorem deserunt. Veniam ut deserunt enim dolore ad consectetur ullamco aute incididunt laborum. Ad et aute veniam labore deserunt in esse laboris consectetur minim ea non irure. Quis fugiat consectetur excepteur incididunt pariatur mollit anim veniam velit labore in duis. Labore aliqua et cupidatat elit dolor cupidatat voluptate anim Lorem fugiat ad. Duis nulla cillum ut in ut laborum sint nisi in fugiat."
            "\n\n"
            "Velit do nulla exercitation cillum enim tempor. Aliqua et elit occaecat occaecat. Ad mollit esse excepteur labore tempor ex fugiat dolor consectetur ut sint eiusmod deserunt. Aliqua elit nostrud voluptate proident laboris ut laboris. Commodo tempor veniam officia magna non aute laboris ipsum id deserunt ex cupidatat anim ea."
            "\n\n"
            "Veniam do consequat proident ex ea cupidatat eiusmod ea cillum duis id mollit et. Qui non nisi culpa deserunt non reprehenderit occaecat cupidatat velit dolore velit eu. Fugiat minim adipisicing aliqua dolor adipisicing velit ea tempor anim nostrud ut."
            "\n\n"
            "Amet excepteur cupidatat labore non labore elit occaecat pariatur tempor veniam. Tempor reprehenderit enim quis veniam culpa aute. Ad sit aute tempor tempor do ut duis tempor aute ea. Non exercitation dolore est cillum ut laborum."
            "\n\n"
            "Dolore exercitation in aliquip Lorem deserunt. Veniam ut deserunt enim dolore ad consectetur ullamco aute incididunt laborum. Ad et aute veniam labore deserunt in esse laboris consectetur minim ea non irure. Quis fugiat consectetur excepteur incididunt pariatur mollit anim veniam velit labore in duis. Labore aliqua et cupidatat elit dolor cupidatat voluptate anim Lorem fugiat ad. Duis nulla cillum ut in ut laborum sint nisi in fugiat."
            "\n\n"
            "Velit do nulla exercitation cillum enim tempor. Aliqua et elit occaecat occaecat. Ad mollit esse excepteur labore tempor ex fugiat dolor consectetur ut sint eiusmod deserunt. Aliqua elit nostrud voluptate proident laboris ut laboris. Commodo tempor veniam officia magna non aute laboris ipsum id deserunt ex cupidatat anim ea."
        );
        igEndChild();

        igPushItemWidth(-1);
        igPushStyleColor_Vec4(ImGuiCol_Button, (ImVec4){0.0f, 0.0f, 0.0f, 0.0f});
        igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, (ImVec4){0.0f, 0.0f, 0.0f, 0.0f});
        igPushStyleColor_Vec4(ImGuiCol_ButtonActive, (ImVec4){0.0f, 0.0f, 0.0f, 0.0f});
        consoleFocusRequestActive |= igButton(">", (ImVec2){15, 20});
        igPopStyleColor(3);
        igSameLine(0.0f, 0.0f);
        if (consoleFocusRequestActive) { igSetKeyboardFocusHere(0); }
        igPushStyleColor_Vec4(ImGuiCol_FrameBg, (ImVec4){0.0f, 0.0f, 0.0f, 0.0f});
        igInputText("", inputBuffer, inputBufferSize, ImGuiInputTextFlags_None, NULL, NULL);
        consoleFocusRequestActive = false;
        igPopStyleColor(1);
        igPopItemWidth();

        igEndChild();
        igEndGroup();

        igEnd();
    }
#endif // defined(DUNGEN_DISABLE_TOOLS)
