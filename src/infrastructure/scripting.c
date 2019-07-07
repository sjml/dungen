#include "../stdafx.h"
#include "scripting.h"


lua_State* L = NULL;

void InitializeLua() {
    L = luaL_newstate();
    luaL_openlibs(L);
}

void FinalizeLua() {
    lua_close(L);
}
