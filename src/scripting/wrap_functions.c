#include "../stdafx.h"
#include "wrap_functions.h"

#include "../world.h"

//vec3 testVec3 = {56.82f, 15.40f, 42.0f};
//
//int lua_getVec3(lua_State* L) {
//    float** pv3 = lua_newuserdata(L, sizeof(float**));
//    *pv3 = testVec3;
//    luaL_setmetatable(L, "vec3");
//    return 1;
//}

int lua_InitializeWorld(lua_State* L) {
    int w = (int)lua_tointeger(L, 1);
    int h = (int)lua_tointeger(L, 2);
    float s = (float)lua_tonumber(L, 3);
    
    InitializeWorld(w, h, s);
    
    return 0;
}


int v3_index(lua_State* L) {
    float** pv3 = luaL_checkudata(L, 1, "vec3");
    int index = (int)luaL_checkinteger(L, 2);
    if (index < 1 || index > 3) {
        fprintf(stderr, "ERROR: invalid index (%d) for vec3\n", index);
        lua_pushnumber(L, 0.0f);
        return 1;
    }
    lua_pushnumber(L, (*pv3)[index-1]);
    return 1;
}

int v3_newindex(lua_State* L) {
    float** pv3 = luaL_checkudata(L, 1, "vec3");
    int index = (int)luaL_checkinteger(L, 2);
    if (index < 1 || index > 3) {
        fprintf(stderr, "ERROR: invalid index (%d) for vec3\n", index);
        lua_pushnumber(L, 0.0f);
        return 1;
    }
    float value = (float)luaL_checknumber(L, 3);
    (*pv3)[index-1] = value;
    return 0;
}

static const luaL_Reg vec3_meta[] = {
    {"__index", v3_index},
    {"__newindex", v3_newindex},
    {NULL, NULL}
};


void WrapFunctionsToScript(lua_State* L) {
    lua_pushcfunction(L, lua_InitializeWorld);
    lua_setglobal(L, "InitializeWorld");
    
    luaL_newmetatable(L, "vec3");
    luaL_setfuncs(L, vec3_meta, 0);
}
