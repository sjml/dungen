%module dungen
%{
    #include "../infrastructure/util.h"
%}



%typemap(in) gbVec4* (gbVec4 tempVec)
{
    // gbVec4 pointer conversion
    gbVec4 *vecPtr;
    if (SWIG_IsOK(SWIG_ConvertPtr(L,$input,(void**)&vecPtr,SWIGTYPE_p_gbVec4,0))) {
        $1 = vecPtr;
    }
    else {
        // make sure it's a table
        if (!lua_istable(L, $input)) {
            SWIG_fail_arg("$symname", $input, "table or native vector");
        }
        if (lua_rawlen(L, $input) < 3) {
            SWIG_fail_arg("$symname", $input, "table with length at least 3");
        }

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
        tempVec.x = x;
        tempVec.y = y;
        tempVec.z = z;
        tempVec.w = w;
        $1 = &tempVec;
    }
}

%typemap(memberin) gbVec4
{
    // gbVec4 member conversion
    $1.x = $input->x;
    $1.y = $input->y;
    $1.z = $input->z;
    $1.w = $input->w;
}

%typemap(in) gbVec4
{
    // gbVec4 conversion
    gbVec4 *vecPtr;
    if (SWIG_IsOK(SWIG_ConvertPtr(L,$input,(void**)&vecPtr,SWIGTYPE_p_gbVec4,0))) {
        $1 = *vecPtr;
    }
    else {
        // make sure it's a table
        if (!lua_istable(L, $input)) {
            SWIG_fail_arg("$symname", $input, "table or native vector");
        }
        if (lua_rawlen(L, $input) < 3) {
            SWIG_fail_arg("$symname", $input, "table with length at least 3");
        }

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
        $1.x = x;
        $1.y = y;
        $1.z = z;
        $1.w = w;
    }
}

%typemap(in) gbVec3* (gbVec3 tempVec)
{
    // gbVec3 pointer conversion
    gbVec3 *vecPtr;
    if (SWIG_IsOK(SWIG_ConvertPtr(L,$input,(void**)&vecPtr,SWIGTYPE_p_gbVec3,0))) {
        $1 = vecPtr;
    }
    else {
        // make sure it's a table
        if (!lua_istable(L, $input)) {
            SWIG_fail_arg("$symname", $input, "table or native vector");
        }
        if (lua_rawlen(L, $input) < 3) {
            SWIG_fail_arg("$symname", $input, "table with length at least 3");
        }

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
        tempVec.x = x;
        tempVec.y = y;
        tempVec.z = z;
        $1 = &tempVec;
    }
}

%typemap(memberin) gbVec3
{
    // gbVec3 member conversion
    $1.x = $input->x;
    $1.y = $input->y;
    $1.z = $input->z;
}

%typemap(in) gbVec3
{
    // gbVec3 conversion
    gbVec3 *vecPtr;
    if (SWIG_IsOK(SWIG_ConvertPtr(L,$input,(void**)&vecPtr,SWIGTYPE_p_gbVec3,0))) {
        $1 = *vecPtr;
    }
    else {
        // make sure it's a table
        if (!lua_istable(L, $input)) {
            SWIG_fail_arg("$symname", $input, "table or native vector");
        }
        if (lua_rawlen(L, $input) < 3) {
            SWIG_fail_arg("$symname", $input, "table with length at least 3");
        }

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
        $1.x = x;
        $1.y = y;
        $1.z = z;
    }
}


%typemap(in) gbVec2* (gbVec2 tempVec)
{
    // gbVec2 pointer conversion
    gbVec2 *vecPtr;
    if (SWIG_IsOK(SWIG_ConvertPtr(L,$input,(void**)&vecPtr,SWIGTYPE_p_gbVec2,0))) {
        $1 = vecPtr;
    }
    else {
        // make sure it's a table
        if (!lua_istable(L, $input)) {
            SWIG_fail_arg("$symname", $input, "table or native vector");
        }
        if (lua_rawlen(L, $input) < 2) {
            SWIG_fail_arg("$symname", $input, "table with length at least 2");
        }

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
        tempVec.x = x;
        tempVec.y = y;
        $1 = &tempVec;
    }
}

%typemap(memberin) gbVec2
{
    // gbVec2 member conversion
    $1.x = $input->x;
    $1.y = $input->y;
}

%typemap(in) gbVec2
{
    // gbVec2 conversion
    gbVec2 *vecPtr;
    if (SWIG_IsOK(SWIG_ConvertPtr(L,$input,(void**)&vecPtr,SWIGTYPE_p_gbVec2,0))) {
        $1 = *vecPtr;
    }
    else {
        // make sure it's a table
        if (!lua_istable(L, $input)) {
            SWIG_fail_arg("$symname", $input, "table or native vector");
        }
        if (lua_rawlen(L, $input) < 2) {
            SWIG_fail_arg("$symname", $input, "table with length at least 2");
        }

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
        $1.x = x;
        $1.y = y;
    }
}


%typemap(in) Vec2i* (Vec2i tempVec)
{
    // Vec2i pointer conversion
    Vec2i *vecPtr;
    if (SWIG_IsOK(SWIG_ConvertPtr(L,$input,(void**)&vecPtr,SWIGTYPE_p_Vec2i,0))) {
        $1 = vecPtr;
    }
    else {
        // make sure it's a table
        if (!lua_istable(L, $input)) {
            SWIG_fail_arg("$symname", $input, "table or native vector");
        }
        if (lua_rawlen(L, $input) < 2) {
            SWIG_fail_arg("$symname", $input, "table with length at least 2");
        }

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
        tempVec.x = x;
        tempVec.y = y;
        $1 = &tempVec;
    }
}

%typemap(memberin) Vec2i
{
    // Vec2i member conversion
    $1.x = $input->x;
    $1.y = $input->y;
}

%typemap(in) Vec2i
{
    // Vec2i conversion
    Vec2i *vecPtr;
    if (SWIG_IsOK(SWIG_ConvertPtr(L,$input,(void**)&vecPtr,SWIGTYPE_p_Vec2i,0))) {
        $1 = *vecPtr;
    }
    else {
        // make sure it's a table
        if (!lua_istable(L, $input)) {
            SWIG_fail_arg("$symname", $input, "table or native vector");
        }
        if (lua_rawlen(L, $input) < 2) {
            SWIG_fail_arg("$symname", $input, "table with length at least 2");
        }

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
        $1.x = x;
        $1.y = y;
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

typedef union gbVec4 {
	struct { float x, y, z, w; };
	struct { float r, g, b, a; };
	struct { gbVec2 xy, zw; };
	gbVec3 xyz;
	gbVec3 rgb;
	float e[4];
} gbVec4;

