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
        return;
    }
    int retVal = (int)lua_tointeger(L, lua_gettop(L));
    lua_pop(L, 1);
    
    if (retVal == 0) {
        HLVMPop(sim);
    }
}
