#include "../stdafx.h"
#include "scripting.h"

// implementation in generated wrapping.c file
int luaopen_dungen(lua_State* L);

// added to the lua library at its build time, but has to
//  get initialized
int luaopen_lfs(lua_State* L);

static lua_State* L = NULL;

void InitializeLua() {
    L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_lfs(L);
    
    luaopen_dungen(L);
    
    RunFile("scripts/sys/init.lua");
}

void FinalizeLua() {
    lua_close(L);
}

lua_State* GetLuaState() {
    return L;
}

int RunFile(const char* filename) {
    int error = luaL_dofile(L, filename);
    if (error) {
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return 1;
    }
    return 0;
}

int RunString(const char* string) {
    int error = luaL_dostring(L, string);
    if (error) {
        fprintf(stderr, "LUA ERROR: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return 1;
    }
    return 0;
}
