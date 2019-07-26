#include "../stdafx.h"
#include "hlvm.h"

#include "../scripting/scripting.h"

SimulationElement** stack = NULL;
long stackTop = -1;

struct { char* key; int value; } *intRegisters = NULL;
struct { char* key; float value; } *floatRegisters = NULL;
struct { char* key; char* value; } *stringRegisters = NULL;
struct { char* key; TileData* value; } *tileRegisters = NULL;
struct { char* key; TileSet* value; } *tileSetRegisters = NULL;


void InitializeHLVM() {
    shdefault(intRegisters, 0);
    shdefault(floatRegisters, 0.0f);
    shdefault(stringRegisters, "");
    shdefault(tileRegisters, NULL);
    shdefault(tileSetRegisters, NULL);
}

SimulationElement* CreateSimulationElement(char* name) {
    lua_State* L = GetLuaState();
    lua_getglobal(L, "VM_wrap");
    lua_pushstring(L, name);
    if (lua_pcall(L, 1, 1, 0) != 0) {
        fprintf(stderr, "LUA ERROR: %s\n", lua_tostring(L, -1));
        return NULL;
    }
    SimulationElement* sim = malloc(sizeof(SimulationElement));
    (*sim).LuaRefKey = luaL_ref(L, LUA_REGISTRYINDEX);
    (*sim).Name = name;
    return sim;
}

void HLVMPush(SimulationElement* sim) {
    if (sim == NULL) {
        fprintf(stderr, "ERROR: Attempt to push null element.\n");
        return;
    }
    arrpush(stack, sim);
    stackTop = arrlen(stack) - 1;
}

void HLVMPop(SimulationElement* topCheck) {
    if (arrlen(stack) == 0) {
        fprintf(stderr, "ERROR: HLVM stack corruption! (trying to pop empty stack)\n");
        return;
    }

    SimulationElement* top = arrpop(stack);
    stackTop--;
    if (top != topCheck) {
        // TODO: have this panic / throw
        fprintf(stderr, "ERROR: HLVM stack corruption! (incorrect top of stack)\n");
        return;
    }
    luaL_unref(GetLuaState(), LUA_REGISTRYINDEX, (*top).LuaRefKey);
    free(top);
}

void HLVMProcess() {
    if (arrlen(stack) == 0) {
        return;
    }

    lua_State* L = GetLuaState();
    SimulationElement* sim = stack[stackTop];

    lua_rawgeti(L, LUA_REGISTRYINDEX, (*sim).LuaRefKey);
    // TODO: should this be pcallk?
    int status = lua_pcall(L, 0, 1, 0);
    if (status != LUA_OK) {
        // error of some kind
        fprintf(stderr, "LUA ERROR: %s; HLVM popping %s\n", lua_tostring(L, -1), (*sim).Name);
        lua_pop(L, 1);
        HLVMPop(sim);
        RunString("ResolveStyles()");
        return;
    }
    int retVal = (int)lua_tointeger(L, lua_gettop(L));
    lua_pop(L, 1);

    if (retVal == 0) {
        HLVMPop(sim);
    }

    RunString("ResolveStyles()");
}


int GetIntRegister(const char* key) {
    return shget(intRegisters, key);
}

float GetFloatRegister(const char* key) {
    return shget(floatRegisters, key);
}

const char* GetStringRegister(const char* key) {
    return shget(stringRegisters, key);
}

TileData* GetTileRegister(const char* key) {
    return shget(tileRegisters, key);
}

TileSet* GetTileSetRegister(const char* key) {
    return shget(tileSetRegisters, key);
}

void SetIntRegister(const char* key, int value) {
    shput(intRegisters, key, value);
}

void SetFloatRegister(const char* key, float value) {
    shput(floatRegisters, key, value);
}

void SetStringRegister(const char* key, char* value) {
    shput(stringRegisters, key, value);
}

void SetTileRegister(const char* key, TileData* value) {
    shput(tileRegisters, key, value);
}

void SetTileSetRegister(const char* key, TileSet* value)  {
    shput(tileSetRegisters, key, value);
}
