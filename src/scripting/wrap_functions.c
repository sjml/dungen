#include "../stdafx.h"
#include "wrap_functions.h"

#include "../world.h"



int lua_InitializeWorld(lua_State* L) {
    int w = (int)lua_tointeger(L, 1);
    int h = (int)lua_tointeger(L, 2);
    float s = (float)lua_tonumber(L, 3);
    
    InitializeWorld(w, h, s);
    
    return 0;
}

int lua_GetTileAtPosition(lua_State* L) {
    int x = (int)lua_tointeger(L, 1);
    int y = (int)lua_tointeger(L, 2);
    
    TileData* td = GetTileAtPosition(x - 1, y - 1);
    
    TileData** ud = lua_newuserdata(L, sizeof(TileData**));
    *ud = td;
    luaL_setmetatable(L, "TileData");
    return 1;
}


int vec3_index(lua_State* L) {
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

int vec3_newindex(lua_State* L) {
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

int vec3_len(lua_State* L) {
    lua_pushinteger(L, 3);
    return 1;
}

int vec3_tostring(lua_State* L) {
    float** pv3 = luaL_checkudata(L, 1, "vec3");
    char buffer[50];
    sprintf(buffer, "[%.2f, %.2f, %.2f]", (*pv3)[0], (*pv3)[1], (*pv3)[2]);
    lua_pushstring(L, buffer);
    return 1;
}

static const luaL_Reg vec3_meta[] = {
    {"__index", vec3_index},
    {"__newindex", vec3_newindex},
    {"__len", vec3_len},
    {"__tostring", vec3_tostring},
    {NULL, NULL}
};



int TileData_index(lua_State* L) {
    TileData** tile = luaL_checkudata(L, 1, "TileData");
    const char* index = luaL_checkstring(L, 2);
    
    // TODO: there must be a better way to do this.
    //    This comment on a SO answer talks about upvalues or storing references
    //    https://stackoverflow.com/questions/19306236/lua-c-api-mapping-a-property-to-a-function#comment28593656_19306568
    if (strcmp(index, "i") == 0) {
        lua_pushinteger(L, (*(*tile)).i);
    }
    else if (strcmp(index, "color") == 0) {
        float** pv3 = lua_newuserdata(L, sizeof(float**));
        *pv3 = (*(*tile)).color;
        luaL_setmetatable(L, "vec3");
    }
    else {
        lua_pushnil(L);
    }
    
    return 1;
}

int TileData_newindex(lua_State* L) {
    TileData** tile = luaL_checkudata(L, 1, "TileData");
    const char* index = luaL_checkstring(L, 2);
    
    // TODO: same note as above
    if (strcmp(index, "color") == 0) {
        // TODO: allow assignment of vec3
        if (!lua_istable(L, 3)) {
            fprintf(stderr, "ERROR: can only assign table to color.\n");
            return 0;
        }
        size_t len = lua_rawlen(L, 3);
        if (len < 3) {
            fprintf(stderr, "ERROR: color assignment table must have three elements.\n");
            return 0;
        }
        lua_pushinteger(L, 1);
        lua_gettable(L, 3);
        (*(*tile)).color[0] = luaL_checknumber(L, -1);
        lua_pop(L, 1);
        
        lua_pushinteger(L, 2);
        lua_gettable(L, 3);
        (*(*tile)).color[1] = luaL_checknumber(L, -1);
        lua_pop(L, 1);
        
        lua_pushinteger(L, 3);
        lua_gettable(L, 3);
        (*(*tile)).color[2] = luaL_checknumber(L, -1);
        lua_pop(L, 1);
    }
    else {
        fprintf(stderr, "ERROR: cannot assign to %s.\n", index);
    }
    
    return 0;
}

static const luaL_Reg TileData_meta[] = {
    {"__index", TileData_index},
    {"__newindex", TileData_newindex},
    {NULL, NULL}
};



void WrapFunctionsToScript(lua_State* L) {
    lua_pushcfunction(L, lua_InitializeWorld);
    lua_setglobal(L, "InitializeWorld");
    
    lua_pushcfunction(L, lua_GetTileAtPosition);
    lua_setglobal(L, "GetTileAtPosition");
    
    luaL_newmetatable(L, "vec3");
    luaL_setfuncs(L, vec3_meta, 0);
    
    luaL_newmetatable(L, "TileData");
    luaL_setfuncs(L, TileData_meta, 0);
}
