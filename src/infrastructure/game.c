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

static double previousTime, currentTime;

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

    previousTime = GetTime();

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

double hlvmAccum = 0.0;
int GameTick(void) {
    previousTime = currentTime;
    currentTime = GetTime();
//    printf("dt: %.4f\n", currentTime - previousTime);
    double dt = gb_clamp(currentTime - previousTime, 0.0, MAX_TIMESTEP);

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

#if !(DUNGEN_MOBILE)
    double GetTime() {
        return glfwGetTime();
    }

    void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
        gbVec2 pos = {(float)xpos, (float)ypos};
        gbVec2 orthoPos = ScreenToOrtho(pos);
        if (GetChoiceStatus() > 0) {
            ChoiceProcessMouseMovement(orthoPos);
        }
        if (GetTileChoiceStatus() > 0) {
            TileChoiceProcessMouseMovement(pos);
        }
    }

    void MouseClickCallback(GLFWwindow* window, int button, int action, int mods) {
        if (GetChoiceStatus() >= 0 && button == GLFW_MOUSE_BUTTON_LEFT) {
            ChoiceProcessMouseClick(action == GLFW_PRESS);
        }
        if (GetTileChoiceStatus() >= 0 && button == GLFW_MOUSE_BUTTON_LEFT) {
            TileChoiceProcessMouseClick(action == GLFW_PRESS);
        }
    }

    void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            shouldStopGame = true;
        }
    }
#else
    double _gameTime;
    void SetTime(double gameTime) {
        _gameTime = gameTime;
    }
    double GetTime() {
        return _gameTime;
    }

    void TouchCallback(double xpos, double ypos, int isDown) {
        gbVec2 pos = {(float)xpos, (float)ypos};
        gbVec2 orthoPos = ScreenToOrtho(pos);
        if (GetChoiceStatus() >= 0) {
            ChoiceProcessMouseMovement(orthoPos);
            ChoiceProcessMouseClick(isDown);
        }
        if (GetTileChoiceStatus() >= 0) {
            TileChoiceProcessMouseMovement(pos);
            TileChoiceProcessMouseClick(isDown);
        }
    }

    void TouchMoveCallback(double xpos, double ypos) {
        gbVec2 pos = {(float)xpos, (float)ypos};
        pos = ScreenToOrtho(pos);
        if (GetChoiceStatus() > 0) {
            ChoiceProcessMouseMovement(pos);
        }
        if (GetTileChoiceStatus() > 0) {
            TileChoiceProcessMouseMovement(pos);
        }
    }
#endif // !(DUNGEN_MOBILE)
