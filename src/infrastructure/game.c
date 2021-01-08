#include "stdafx.h"
#include "game.h"

#include <time.h>
#include <pcg_basic.h>

#include "util.h"
#include "world.h"
#include "rendering.h"
#include "../scripting/scripting.h"
#include "../hlvm/hlvm.h"
#include "../ui/banner.h"
#include "../ui/choice.h"
#include "../ui/tile_choice.h"

#define MAX_TIMESTEP      1.0
#define TICKS_PER_CYCLE   1
#define SECONDS_PER_CYCLE 0.0

static uint64_t previousTime;

static sapp_event* frameEvents = NULL;
static gbVec2 mousePos;

static pcg32_random_t randomState;
sds randomSeedString = NULL;

static bool shouldStopGame = false;

void SeedRandomString(const char* seedString) {
    int count;
    sds* toks = sdssplitlen(seedString, strlen(seedString), "///", 3, &count);
    if (count != 2) {
        sdsfreesplitres(toks, count);
        fprintf(stderr, "ERROR: Invalid random seed. Falling back to standard seeding procedure.\n");
        SeedRandom(0, 0);
        return;
    }
    uint64_t seed = strtoull(toks[0], NULL, 10);
    uint64_t seq  = strtoull(toks[1], NULL, 10);
    SeedRandom(seed, seq);
    sdsfreesplitres(toks, count);
}

void SeedRandom(uint64_t seed, uint64_t seq) {
    // This is not the most random thing we could do, but
    //   we don't need crypto-level randomness here.
    if (seed == 0 && seq == 0) {
        seed = time(NULL) ^ (intptr_t)&printf;
        seq = (intptr_t)&previousTime;
    }
    if (randomSeedString != NULL) {
        sdsfree(randomSeedString);
    }
    randomSeedString = sdsempty();
    randomSeedString = sdscatprintf(randomSeedString, "%llu///%llu", seed, seq);
    printf("Random Seed: %s\n\n", randomSeedString);
    pcg32_srandom_r(&randomState, seed, seq);
}

const char* GetRandomSeed() {
    return randomSeedString;
}

int RandomInt(int max) {
    return (int)pcg32_boundedrand_r(&randomState, max);
}

// modified from gb_math
float RandomRangeFloat(float min, float max) {
    int int_result = RandomRangeInt(0, 2147483646);
    float result = int_result/(float)2147483646;
    result *= max - min;
    result += min;
    return result;
}

int RandomRangeInt(int min, int max) {
    int result = RandomInt(max - min) + min;
    return result;
}

void InitializeGame(const char* startupElement) {
    SeedRandom(0, 0);

    stm_setup();
    previousTime = stm_now();
    mousePos = (gbVec2){-1.0f, -1.0f};

    InitializeHLVM();

    if (RunFile("scripts/simulation/WorldSetup.lua") == 0) {
        lua_getglobal(GetLuaState(), "push");
        lua_pushstring(GetLuaState(), startupElement);
        if (lua_pcall(GetLuaState(), 1, 0, 0) != 0) {
            fprintf(stderr, "ERROR: No global 'push()' function defined in Lua!\n");
        }
    }
}

void FinalizeGame(void) {
    FinalizeWorld();
    sdsfree(randomSeedString);
}

void QuitGame(const char* message) {
    if (message != NULL) {
        printf("Quitting: %s\n", message);
    }
    sapp_request_quit();
}

void ProcessEvent(const sapp_event* event) {
    sapp_event ev_cpy = *event;
    arrpush(frameEvents, ev_cpy);
}

void _RunEvents() {
    for (int ei=0; ei < arrlen(frameEvents); ei++) {
        sapp_event *event = &frameEvents[ei];
        if (event->type == SAPP_EVENTTYPE_KEY_DOWN) {
            if (event->key_code == SAPP_KEYCODE_ESCAPE) {
                sapp_request_quit();
            }
        }
        else if (event->type == SAPP_EVENTTYPE_CHAR) {
            // no-op
        }
        else if (event->type == SAPP_EVENTTYPE_MOUSE_MOVE) {
            mousePos.x = event->mouse_x / sapp_dpi_scale();
            mousePos.y = event->mouse_y / sapp_dpi_scale();

            if (GetChoiceStatus() > 0) {
                ChoiceProcessMouseMovement(ScreenToOrtho(mousePos));
            }
            if (GetTileChoiceStatus() > 0) {
                TileChoiceProcessMouseMovement(mousePos);
            }
        }
        else if (event->type == SAPP_EVENTTYPE_MOUSE_DOWN || event->type == SAPP_EVENTTYPE_MOUSE_UP) {
            if (GetChoiceStatus() >= 0 && event->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
                ChoiceProcessMouseClick(event->type == SAPP_EVENTTYPE_MOUSE_DOWN);
            }
            if (GetTileChoiceStatus() >= 0 && event->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
                TileChoiceProcessMouseClick(event->type == SAPP_EVENTTYPE_MOUSE_DOWN);
            }
        }
        else if (event->type == SAPP_EVENTTYPE_MOUSE_ENTER) {
            mousePos.x = event->mouse_x / sapp_dpi_scale();
            mousePos.y = event->mouse_y / sapp_dpi_scale();
        }
        else if (event->type == SAPP_EVENTTYPE_MOUSE_LEAVE) {
            // no-op
        }
        else if (event->type == SAPP_EVENTTYPE_QUIT_REQUESTED) {
            // no-op
        }
        else if (event->type == SAPP_EVENTTYPE_RESIZED) {
            UpdateRenderingDimensions();
        }
        #if DEBUG
        else {
            printf("Unprocessed event!\n");
        }
        #endif // DEBUG
    }
    arrfree(frameEvents);
    frameEvents = NULL;
}

double GetTime(void) {
    return stm_sec(stm_now());
}

double hlvmAccum = 0.0;
int GameTick(void) {
    _RunEvents(); // process input since last frame (safe to call gfx functions now)

    uint64_t dt_ticks = stm_laptime(&previousTime);
    double dt = stm_sec(dt_ticks);
    // printf("dt: %.4f\n", dt);
    dt = gb_clamp(dt, 0.0, MAX_TIMESTEP);

    bool waiting = GetIntRegister("WaitForUI") != 0;
    if (!waiting) {
        hlvmAccum += dt;
        if (hlvmAccum >= SECONDS_PER_CYCLE) {
            hlvmAccum = 0.0;
            int hlvmTickCount = 0;
            while (hlvmTickCount++ < TICKS_PER_CYCLE) {
                lua_getglobal(GetLuaState(), "HLVMProcess");
                if (!lua_isfunction(GetLuaState(), -1)) {
                    fprintf(stderr, "ERROR: No global 'HLVMProcess()' function defined in Lua!\n");
                }
                if (lua_pcall(GetLuaState(), 0, 0, 0) != 0) {
                    fprintf(stderr, "ERROR: %s\n", lua_tostring(GetLuaState(), -1));
                }
            }
        }
    }

    if (waiting) {
        bool done = true;
        if (UpdateBanners((float)dt)) {
            done = false;
        }
        if (GetChoiceStatus() > 0) {
            done = false;
        }
        if (GetTileChoiceStatus() > 0) {
            done = false;
        }
        if (done) {
            SetIntRegister("WaitForUI", 0);
        }
    }

    return shouldStopGame;
}

gbVec2 GetCursorPosition(void) {
    return mousePos;
}
//
//#if !(DUNGEN_MOBILE)
//    double GetTime() {
//        return glfwGetTime();
//    }
//
//    void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
//        gbVec2 pos = {(float)xpos, (float)ypos};
//        gbVec2 orthoPos = ScreenToOrtho(pos);
//        if (GetChoiceStatus() > 0) {
//            ChoiceProcessMouseMovement(orthoPos);
//        }
//        if (GetTileChoiceStatus() > 0) {
//            TileChoiceProcessMouseMovement(pos);
//        }
//    }
//
//    void MouseClickCallback(GLFWwindow* window, int button, int action, int mods) {
//        if (GetChoiceStatus() >= 0 && button == GLFW_MOUSE_BUTTON_LEFT) {
//            ChoiceProcessMouseClick(action == GLFW_PRESS);
//        }
//        if (GetTileChoiceStatus() >= 0 && button == GLFW_MOUSE_BUTTON_LEFT) {
//            TileChoiceProcessMouseClick(action == GLFW_PRESS);
//        }
//    }
//
//    void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
//        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
//            shouldStopGame = true;
//        }
//    }
//#else
//    double _gameTime;
//    void SetTime(double gameTime) {
//        _gameTime = gameTime;
//    }
//    double GetTime() {
//        return _gameTime;
//    }
//
//    void TouchCallback(double xpos, double ypos, int isDown) {
//        gbVec2 pos = {(float)xpos, (float)ypos};
//        gbVec2 orthoPos = ScreenToOrtho(pos);
//        if (GetChoiceStatus() >= 0) {
//            ChoiceProcessMouseMovement(orthoPos);
//            ChoiceProcessMouseClick(isDown);
//        }
//        if (GetTileChoiceStatus() >= 0) {
//            TileChoiceProcessMouseMovement(pos);
//            TileChoiceProcessMouseClick(isDown);
//        }
//    }
//
//    void TouchMoveCallback(double xpos, double ypos) {
//        gbVec2 pos = {(float)xpos, (float)ypos};
//        pos = ScreenToOrtho(pos);
//        if (GetChoiceStatus() > 0) {
//            ChoiceProcessMouseMovement(pos);
//        }
//        if (GetTileChoiceStatus() > 0) {
//            TileChoiceProcessMouseMovement(pos);
//        }
//    }
//#endif // !(DUNGEN_MOBILE)
