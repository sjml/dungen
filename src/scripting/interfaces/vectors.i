%module dungen
%{
    #include <gb_math.h>
    #include "../infrastructure/util.h"
%}

%typemap(in) gbVec4*
{
    // gbVec4 conversion
    gbVec4 *vecPtr;
    gbVec4 vec;
    if (SWIG_IsOK(SWIG_ConvertPtr(L,$input,(void**)&vecPtr,SWIGTYPE_p_gbVec4,0))) {
        $1 = vecPtr;
    }
    else {
        // convert table parameters to floats
        lua_pushinteger(L, 1);
        lua_gettable(L, $input);
        float x = lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushinteger(L, 2);
        lua_gettable(L, $input);
        float y = lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushinteger(L, 3);
        lua_gettable(L, $input);
        float z = lua_tonumber(L, -1);
        lua_pop(L, 1);

        float w = 1.0f;
        if (lua_rawlen(L, $input) >= 4) {
            lua_pushinteger(L, 4);
            lua_gettable(L, $input);
            w = lua_tonumber(L, -1);
            lua_pop(L, 1);
        }

        // build the vector
        vec.x = x;
        vec.y = y;
        vec.z = z;
        vec.w = w;
        $1 = &vec;
    }
}

%typemap(in) gbVec3*
{
    // gbVec3 conversion
    gbVec3 *vecPtr;
    gbVec3 vec;
    if (SWIG_IsOK(SWIG_ConvertPtr(L,$input,(void**)&vecPtr,SWIGTYPE_p_gbVec3,0))) {
        $1 = vecPtr;
    }
    else {
        // convert table parameters to floats
        lua_pushinteger(L, 1);
        lua_gettable(L, $input);
        float x = lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushinteger(L, 2);
        lua_gettable(L, $input);
        float y = lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushinteger(L, 3);
        lua_gettable(L, $input);
        float z = lua_tonumber(L, -1);
        lua_pop(L, 1);

        // build the vector
        vec.x = x;
        vec.y = y;
        vec.z = z;
        $1 = &vec;
    }
}

%typemap(in) gbVec2*
{
    // gbVec2 conversion
    gbVec2 *vecPtr;
    gbVec2 vec;
    if (SWIG_IsOK(SWIG_ConvertPtr(L,$input,(void**)&vecPtr,SWIGTYPE_p_gbVec2,0))) {
        $1 = vecPtr;
    }
    else {
        // convert table parameters to floats
        lua_pushinteger(L, 1);
        lua_gettable(L, $input);
        float x = lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushinteger(L, 2);
        lua_gettable(L, $input);
        float y = lua_tonumber(L, -1);
        lua_pop(L, 1);

        // build the vector
        vec.x = x;
        vec.y = y;
        $1 = &vec;
    }
}

%typemap(in) Vec2i*
{
    // Vec2i conversion
    Vec2i *vecPtr;
    Vec2i vec;
    if (SWIG_IsOK(SWIG_ConvertPtr(L,$input,(void**)&vecPtr,SWIGTYPE_p_Vec2i,0))) {
        $1 = vecPtr;
    }
    else {
        // convert table parameters to floats
        lua_pushinteger(L, 1);
        lua_gettable(L, $input);
        int x = (int)lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_pushinteger(L, 2);
        lua_gettable(L, $input);
        int y = (int)lua_tointeger(L, -1);
        lua_pop(L, 1);

        // build the vector
        vec.x = x;
        vec.y = y;
        $1 = &vec;
    }
}



typedef struct {
    int x, y;
} Vec2i;

typedef union gbVec2 {
    struct { float x, y; };
    float e[2];
} gbVec2;

typedef union gbVec3 {
    struct { float x, y, z; };
    struct { float r, g, b; };

    gbVec2 xy;
    float e[3];
} gbVec3;

