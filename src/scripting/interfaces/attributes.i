%module dungen
%{
    #include "../infrastructure/attributes.h"
%}

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

typedef enum {
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual,
    Equal,
    NotEqual
} AttrComparison;

TileData** GetTilesByAttribute(const char* attrName, AttrComparison comp, const char* value);
bool CheckTileAttribute(TileData* td, const char* attrName, AttrComparison comp, const char* value);

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
