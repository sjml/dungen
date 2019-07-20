%module dungen
%{
    #include "../infrastructure/attributes.h"
%}

void SetTileAttributeInt(TileData* data, char* name, int value);
void SetTileAttributeFloat(TileData* data, char* name, float value);
void SetTileAttributeString(TileData* data, char* name, char* value);

int GetTileAttributeInt(TileData* data, char* name);
float GetTileAttributeFloat(TileData* data, char* name);
char* GetTileAttributeString(TileData* data, char* name);

bool AddTileTag(TileData* td, char* tag);
bool RemoveTileTag(TileData* td, char* tag);
TileData** GetTilesTagged(char* tagString);
bool TileHasTags(TileData* td, char* tagString);
char** GetTileTags(TileData* td);


void SetTileSetAttributeInt(TileSet* data, char* name, int value);
void SetTileSetAttributeFloat(TileSet* data, char* name, float value);
void SetTileSetAttributeString(TileSet* data, char* name, char* value);

int GetTileSetAttributeInt(TileSet* data, char* name);
float GetTileSetAttributeFloat(TileSet* data, char* name);
char* GetTileSetAttributeString(TileSet* data, char* name);

bool AddTileSetTag(TileSet* td, char* tag);
bool RemoveTileSetTag(TileSet* td, char* tag);
TileSet** GetTileSetsTagged(char* tagString);
bool TileSetHasTags(TileSet* td, char* tagString);
char** GetTileSetTags(TileSet* td);
