#pragma once

#include "../infrastructure/world.h"

typedef struct {
    int LuaRefKey;
    char* Name;
} SimulationElement;

void InitializeHLVM(void);

SimulationElement* CreateSimulationElement(char* name);
void HLVMPush(SimulationElement* sim);
void HLVMProcess(void);

int         GetIntRegister     (const char* key);
float       GetFloatRegister   (const char* key);
const char* GetStringRegister  (const char* key);
TileData*   GetTileRegister    (const char* key);
TileSet*    GetTileSetRegister (const char* key);

void SetIntRegister     (const char* key, int       value);
void SetFloatRegister   (const char* key, float     value);
void SetStringRegister  (const char* key, char*     value);
void SetTileRegister    (const char* key, TileData* value);
void SetTileSetRegister (const char* key, TileSet*  value);
