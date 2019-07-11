#include "../stdafx.h"
#include "hlvm.h"

#include "../scripting/scripting.h"

SimulationElement** stack = NULL;
long stackTop = -1;

void InitializeHLVM() {

}

SimulationElement* CreateSimulationElement(const char* name) {
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

SimulationElement* HLVMPop() {
    if (arrlen(stack) == 0) {
        return NULL;
    }
    stackTop -= 1;
    return arrpop(stack);
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
        fprintf(stderr, "LUA ERROR: %s; popping %s\n", lua_tostring(L, -1), (*sim).Name);
        lua_pop(L, 1);
        return;
    }
    int retVal = (int)lua_tointeger(L, lua_gettop(L));
    lua_pop(L, 1);
    
    if (retVal == 0) {
        SimulationElement* check = arrpop(stack);
        stackTop--;
        if (check != sim) {
            // TODO: have this panic / throw
            fprintf(stderr, "ERROR: HLVM stack corruption!\n");
            return;
        }
        luaL_unref(L, LUA_REGISTRYINDEX, (*sim).LuaRefKey);
        free(sim);
    }
}
