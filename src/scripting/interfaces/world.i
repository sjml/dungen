%module dungen
%{
    #include "../infrastructure/world.h"
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
        arrfree($1);

        SWIG_arg += 1;
    }
%}

%nodefaultdtor TileData;
typedef struct {
    long long i;
    Vec2i hexPos;
    gbVec3 color;

    int neighborW;
    int neighborNW;
    int neighborNE;
    int neighborE;
    int neighborSE;
    int neighborSW;

    TileSet** memberSets;
} TileData;

%nodefaultctor TileSet;
%nodefaultdtor TileSet;
typedef struct sTileSet {
    long long i;
    Outline* outline;
} TileSet;

%extend sTileSet {
    sTileSet() {
        return CreateTileSet();
    }
    ~sTileSet() {
        DestroyTileSet($self);
    }
}

void InitializeWorld(int width, int height, float scale);
Vec2i GetWorldDimensions(void);
float GetWorldScale(void);

TileData* GetTileAtPosition(int x, int y);
TileData* GetTileAtIndex(long long i);
// GetTileNeighbors intentionally omitted in favor of Lua version
// TileData** GetTileNeighbors(TileData* center, int *numNeighbors);

int AddTileToSet(TileSet* ts, TileData* t);
int RemoveTileFromSet(TileSet* ts, TileData* t);
bool IsTileInSet(TileSet* ts, TileData* t);
int GetTileCount(TileSet* ts);
TileData** GetTiles(TileSet* ts);
// DestroyTileSet intentionally omitted in favor of Lua destructor
// void DestroyTileSet(TileSet* ts);
