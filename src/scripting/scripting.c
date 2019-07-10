#include "../stdafx.h"
#include "scripting.h"

// implementation in generated wrapping.c file
int luaopen_dungen(lua_State* L);

lua_State* L = NULL;

void InitializeLua() {
    L = luaL_newstate();
    luaL_openlibs(L);
    
    luaopen_dungen(L);
    
    int error = luaL_dofile(L, "scripts/init.lua");
    if (error) {
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    else {
        // printf("Lua scripting initialized.\n");
    }
}

void FinalizeLua() {
    lua_close(L);
}
