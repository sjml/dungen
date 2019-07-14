#pragma once

#include "../world.h"

void InitializeAttributes(void);
void FinalizeAttributes(void);

void SetupAttributeData(TileData* data);

void SetAttributeInt(TileData* data, char* name, int value);
void SetAttributeFloat(TileData* data, char* name, float value);
void SetAttributeString(TileData* data, char* name, char* value);

int GetAttributeInt(TileData* data, char* name);
float GetAttributeFloat(TileData* data, char* name);
char* GetAttributeString(TileData* data, char* name);

bool AddTag(TileData* td, char* tag);
bool RemoveTag(TileData* td, char* tag);
TileData** GetTilesTagged(char* tagString);
char** GetTags(TileData* td);
