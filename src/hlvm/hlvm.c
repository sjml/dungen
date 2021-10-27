#include "stdafx.h"
#include "hlvm.h"

#include "../scripting/scripting.h"

static struct { char* key; int value; } *intRegisters = NULL;
static struct { char* key; float value; } *floatRegisters = NULL;
static struct { char* key; char* value; } *stringRegisters = NULL;
static struct { char* key; TileData* value; } *tileRegisters = NULL;
static struct { char* key; TileSet* value; } *tileSetRegisters = NULL;


void InitializeHLVM() {
    sh_new_arena(intRegisters);
    shdefault(intRegisters, 0);
    sh_new_arena(floatRegisters);
    shdefault(floatRegisters, 0.0f);
    sh_new_arena(stringRegisters);
    shdefault(stringRegisters, "");
    sh_new_arena(tileRegisters);
    shdefault(tileRegisters, NULL);
    sh_new_arena(tileSetRegisters);
    shdefault(tileSetRegisters, NULL);
}

void FinalizeHLVM() {
    if (intRegisters != NULL) {
        shfree(intRegisters);
    }
    if (floatRegisters != NULL) {
        shfree(floatRegisters);
    }
    if (stringRegisters != NULL) {
        shfree(stringRegisters);
    }
    if (tileRegisters != NULL) {
        shfree(tileRegisters);
    }
    if (tileSetRegisters != NULL) {
        shfree(tileSetRegisters);
    }
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

char** ListIntRegisters() {
    char** names = NULL;
    for (int i=0; i < shlen(intRegisters); i++) {
        arrpush(names, intRegisters[i].key);
    }
    return names;
}

char** ListFloatRegisters() {
    char** names = NULL;
    for (int i=0; i < shlen(floatRegisters); i++) {
        arrpush(names, floatRegisters[i].key);
    }
    return names;
}

char** ListStringRegisters() {
    char** names = NULL;
    for (int i=0; i < shlen(stringRegisters); i++) {
        arrpush(names, stringRegisters[i].key);
    }
    return names;
}

char** ListTileRegisters() {
    char** names = NULL;
    for (int i=0; i < shlen(tileRegisters); i++) {
        arrpush(names, tileRegisters[i].key);
    }
    return names;
}

char** ListTileSetRegisters() {
    char** names = NULL;
    for (int i=0; i < shlen(tileSetRegisters); i++) {
        arrpush(names, tileSetRegisters[i].key);
    }
    return names;
}

