%module dungen
%{
    #include "../infrastructure/attributes.h"
%}

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

void SetAttributeInt(TileData* data, char* name, int value);
void SetAttributeFloat(TileData* data, char* name, float value);
void SetAttributeString(TileData* data, char* name, char* value);

int GetAttributeInt(TileData* data, char* name);
float GetAttributeFloat(TileData* data, char* name);
char* GetAttributeString(TileData* data, char* name);

bool AddTag(TileData* td, char* tag);
bool RemoveTag(TileData* td, char* tag);
TileData** GetTilesTagged(char* tagString);
bool TileHasTags(TileData* td, char* tagString);
char** GetTags(TileData* td);
