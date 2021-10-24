#include "stdafx.h"
#include "hlvm.h"

#include "../scripting/scripting.h"

static struct { char* key; int value; } *intRegisters = NULL;
static struct { char* key; float value; } *floatRegisters = NULL;
static struct { char* key; char* value; } *stringRegisters = NULL;
static struct { char* key; TileData* value; } *tileRegisters = NULL;
static struct { char* key; TileSet* value; } *tileSetRegisters = NULL;


void InitializeHLVM() {
    shdefault(intRegisters, 0);
    shdefault(floatRegisters, 0.0f);
    shdefault(stringRegisters, "");
    shdefault(tileRegisters, NULL);
    shdefault(tileSetRegisters, NULL);
}

void PushSimulationElement(const char* el) {
    lua_getglobal(GetLuaState(), "push");
    lua_pushstring(GetLuaState(), el);
    if (lua_pcall(GetLuaState(), 1, 0, 0) != 0) {
        fprintf(stderr, "ERROR: No global 'push()' function defined in Lua!\n");
    }
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
