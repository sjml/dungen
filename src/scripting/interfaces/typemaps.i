%module dungen


%typemap(out) char**
%{
    {
        lua_newtable(L);
        if (arrlen($1) > 0) {
            for (unsigned int i=1; i <= arrlen($1); i++) {
                lua_pushnumber(L, i);
                lua_pushstring(L, $1[i-1]);
                lua_settable(L, -3);
            }
        }
        for (int i=0; i < arrlen($1); i++) {
            free($1[i]);
        }
        arrfree($1);
        
        SWIG_arg += 1;
    }
%}

%typemap(out) TileData**
%{
    {
        lua_newtable(L);
        if (arrlen($1) > 0) {
            for (unsigned int i=1; i <= arrlen($1); i++) {
                lua_pushnumber(L, i);
                SWIG_NewPointerObj(L, $1[i-1], SWIGTYPE_p_TileData, 1);
                lua_settable(L, -3);
            }
        }
        arrfree($1);
        
        SWIG_arg += 1;
    }
%}

%typemap(out) TileSet**
%{
    {
        lua_newtable(L);
        if (arrlen($1) > 0) {
            for (unsigned int i=1; i <= arrlen($1); i++) {
                lua_pushnumber(L, i);
                SWIG_NewPointerObj(L, $1[i-1], SWIGTYPE_p_sTileSet, 0);
                lua_settable(L, -3);
            }
        }
        // arrfree($1); // don't free if we don't own this memory
        
        SWIG_arg += 1;
    }
%}
