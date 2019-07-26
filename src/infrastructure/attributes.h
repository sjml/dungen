#pragma once

#include "world.h"

void InitializeAttributes(void);
void FinalizeAttributes(void);

void SetupTileAttributeData(TileData* data);
long long SetupTileSetAttributeData(TileSet* ts);
void ClearTileSetAttributeData(TileSet* ts);

void SetTileAttributeInt(TileData* data, const char* name, int value);
void SetTileAttributeFloat(TileData* data, const char* name, float value);
void SetTileAttributeString(TileData* data, const char* name, const char* value);

int GetTileAttributeInt(TileData* data, const char* name);
float GetTileAttributeFloat(TileData* data, const char* name);
char* GetTileAttributeString(TileData* data, const char* name);

bool AddTileTag(TileData* td, char* tag);
bool RemoveTileTag(TileData* td, const char* tag);
TileData** GetTilesTagged(const char* tagString);
bool TileHasTags(TileData* td, const char* tagString);
char** GetTileTags(TileData* td);


void SetTileSetAttributeInt(TileSet* data, const char* name, int value);
void SetTileSetAttributeFloat(TileSet* data, const char* name, float value);
void SetTileSetAttributeString(TileSet* data, const char* name, const char* value);

int GetTileSetAttributeInt(TileSet* data, const char* name);
float GetTileSetAttributeFloat(TileSet* data, const char* name);
char* GetTileSetAttributeString(TileSet* data, const char* name);

bool AddTileSetTag(TileSet* td, char* tag);
bool RemoveTileSetTag(TileSet* td, const char* tag);
TileSet** GetTileSetsTagged(const char* tagString);
bool TileSetHasTags(TileSet* td, const char* tagString);
char** GetTileSetTags(TileSet* td);
