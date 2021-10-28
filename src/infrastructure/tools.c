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
#include "log.h"
#include "../hlvm/hlvm.h"
#include "../scripting/scripting.h"


static bool toolsVisible = false;
static FileInfo* scriptListing = NULL;
static char* inputBuffer = NULL;
static int inputBufferSize = 1024;
static bool consoleFocusRequestActive = false;


static size_t lastConsoleTextSize = 0;
static sds consoleText;
static char** consoleHistory = NULL;
static int consoleHistoryIndex = -1;

void ConsoleLog(char* out) {
    consoleText = sdscat(consoleText, out);
}

void ConsoleErrorLog(char* out) {
    // TODO: color this differently if possible?
    consoleText = sdscat(consoleText, out);
}

void InitializeTools(void) {
    if (scriptListing != NULL) {
        FileInfoFree(scriptListing);
    }
    scriptListing = GetFileSystemInformation("scripts/simulation/elements");

    inputBuffer = malloc(inputBufferSize);
    inputBuffer[0] = '\0';

    consoleText = sdsempty();
    consoleText = sdscat(consoleText, LUA_COPYRIGHT);
    consoleText = sdscat(consoleText, "\n\n");
    lastConsoleTextSize = sdslen(consoleText);

    RegisterLogFunction(ConsoleLog, 0);
    RegisterLogFunction(ConsoleErrorLog, 1);
}

void FinalizeTools(void) {
    if (scriptListing != NULL) {
        FileInfoFree(scriptListing);
    }

    sdsfree(consoleText);
    lastConsoleTextSize = 0;

    for (int i=0; i < arrlen(consoleHistory); i++) {
        free(consoleHistory[i]);
    }
    arrfree(consoleHistory);
    consoleHistoryIndex = -1;

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

    int ConsoleInputCallback(ImGuiInputTextCallbackData* cbData) {
        if (cbData->EventFlag == ImGuiInputTextFlags_CallbackHistory) {
            const int currentHistoryIndex = consoleHistoryIndex;
            if (cbData->EventKey == ImGuiKey_UpArrow) {
                if (consoleHistoryIndex == -1) {
                    consoleHistoryIndex = (int)arrlen(consoleHistory) - 1;
                }
                else if (consoleHistoryIndex > 0) {
                    consoleHistoryIndex -= 1;
                }
            }
            else if (cbData->EventKey == ImGuiKey_DownArrow) {
                if (consoleHistoryIndex != -1) {
                    if (++consoleHistoryIndex >= arrlen(consoleHistory)) {
                        consoleHistoryIndex = -1;
                    }
                }
            }

            if (currentHistoryIndex != consoleHistoryIndex) {
                const char* rep = (consoleHistoryIndex >= 0) ? consoleHistory[consoleHistoryIndex] : "";
                ImGuiInputTextCallbackData_DeleteChars(cbData, 0, cbData->BufTextLen);
                ImGuiInputTextCallbackData_InsertChars(cbData, 0, rep, NULL);
            }
        }
        return 0;
    }

    void RenderTools(void) {
        if (!toolsVisible) {
            return;
        }

        igSetNextWindowPos((ImVec2){10,10}, ImGuiCond_Once, (ImVec2){0,0});
        igSetNextWindowSizeConstraints((ImVec2){1004, 500}, (ImVec2){1004, 748}, NULL, NULL);
        igBegin("DunGen Tools", 0, ImGuiNextWindowDataFlags_None
            // | ImGuiWindowFlags_AlwaysAutoResize
            // | ImGuiWindowFlags_NoMove
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
        igTextWrapped(consoleText);
        if (lastConsoleTextSize != sdslen(consoleText)) {
            igSetScrollHereY(1.0f);
        }
        lastConsoleTextSize = sdslen(consoleText);
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
        if (igInputText("", inputBuffer, inputBufferSize, ImGuiInputTextFlags_None
            | ImGuiInputTextFlags_EnterReturnsTrue
            | ImGuiInputTextFlags_CallbackHistory
            ,
            ConsoleInputCallback, NULL
        )) {
            if (strlen(inputBuffer) > 0) {
                char* logInput = malloc(sizeof(char) * (strlen(inputBuffer) + 1 + 3)); // "> " and newline and terminator
                sprintf(logInput, "> %s\n", inputBuffer);
                ConsoleLog(logInput);
                free(logInput);
                RunString(inputBuffer);
                if (arrlen(consoleHistory) == 0) {
                    arrpush(consoleHistory, strdup(inputBuffer));
                }
                else if (strcmp(consoleHistory[arrlen(consoleHistory)-1], inputBuffer) != 0) {
                    arrpush(consoleHistory, strdup(inputBuffer));
                }
                consoleHistoryIndex = (int)arrlen(consoleHistory);
                inputBuffer[0] = '\0';
            }
            consoleFocusRequestActive = true;
        }
        else {
            consoleFocusRequestActive = false;
        }
        igPopStyleColor(1);
        igPopItemWidth();

        igEndChild();
        igEndGroup();

        igEnd();
    }
#endif // defined(DUNGEN_DISABLE_TOOLS)
