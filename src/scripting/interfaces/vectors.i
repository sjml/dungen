%module dungen
%{
    #include <gb_math.h>
%}

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

