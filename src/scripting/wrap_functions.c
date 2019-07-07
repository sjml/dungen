#include "../stdafx.h"
#include "wrap_functions.h"

#include "../world.h"


static int l_initWorld(lua_State* L) {
    int w = (int)lua_tointeger(L, 1);
    int h = (int)lua_tointeger(L, 2);
    float s = (float)lua_tonumber(L, 3);
    
    InitializeWorld(w, h, s);
    
    return 0;
}

void WrapFunctionsToScript(lua_State* L) {
    lua_pushcfunction(L, l_initWorld);
    lua_setglobal(L, "InitializeWorld");
}
