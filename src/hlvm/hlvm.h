#pragma once

#include "../infrastructure/world.h"

void InitializeHLVM(void);
void FinalizeHLVM(void);

void PushSimulationElement(const char* el);

int         GetIntRegister     (const char* key);
float       GetFloatRegister   (const char* key);
const char* GetStringRegister  (const char* key);
TileData*   GetTileRegister    (const char* key);
TileSet*    GetTileSetRegister (const char* key);

void SetIntRegister     (const char* key, int          value);
void SetFloatRegister   (const char* key, float        value);
void SetStringRegister  (const char* key, char*        value);
void SetTileRegister    (const char* key, TileData*    value);
void SetTileSetRegister (const char* key, TileSet*     value);

char** ListIntRegisters();
