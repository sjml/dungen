%module dungen
%{
    #include "../hlvm/hlvm.h"
%}

typedef struct {
    int LuaRefKey;
    char* Name;
} SimulationElement;

SimulationElement* CreateSimulationElement(char* name);
void HLVMPush(SimulationElement* sim);
SimulationElement* HLVMPop(void);

int         GetIntRegister     (const char* key);
float       GetFloatRegister   (const char* key);
const char* GetStringRegister  (const char* key);
TileData*   GetTileRegister    (const char* key);
TileSet*    GetTileSetRegister (const char* key);

void SetIntRegister     (const char* key, int         value);
void SetFloatRegister   (const char* key, float       value);
void SetStringRegister  (const char* key, char*       value);
void SetTileRegister    (const char* key, TileData*   value);
void SetTileSetRegister (const char* key, TileSet*    value);
