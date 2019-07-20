#include "../stdafx.h"
#include "attributes.h"

// Doing all this with SQLite may be... overkill?
//     Using a sledgehammer to crack a walnut?
//     Whatever. It's done. Here it is.
#include <sqlite3.h>

sqlite3* db;

typedef enum eAttrType {
    TILE,
    TILESET
} AttrType;

void InitializeAttributes() {
    char* err = 0;
    const char* dbPath = ":memory:";

    int ret = sqlite3_open(dbPath, &db);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    ret = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, &err);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: %s\n", err);
        sqlite3_free(err);
        sqlite3_close(db);
        return;
    }

    char *creation =
        "CREATE TABLE tiles(tile_id INTEGER PRIMARY KEY, ptr INTEGER);"
        "CREATE TABLE tilesets(tileset_id INTEGER PRIMARY KEY, ptr INTEGER);"
        "CREATE TABLE tags(tag_id INTEGER PRIMARY KEY, value STRING UNIQUE);"
        "CREATE INDEX tagIdx ON tags(value);"
        "CREATE TABLE tiles_tags(tile_id INTEGER, tag_id INTEGER, "
            "FOREIGN KEY(tile_id) REFERENCES tiles(tile_id), "
            "FOREIGN KEY(tag_id) REFERENCES tags(tag_id), "
            "PRIMARY KEY(tile_id, tag_id)"
        ");"
        "CREATE TABLE tilesets_tags(tileset_id INTEGER, tag_id INTEGER, "
            "FOREIGN KEY(tileset_id) REFERENCES tilesets(tileset_id), "
            "FOREIGN KEY(tag_id) REFERENCES tags(tag_id), "
            "PRIMARY KEY(tileset_id, tag_id)"
        ");"
    ;
    ret = sqlite3_exec(db, creation, 0, 0, &err);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: %s\n", err);
        sqlite3_free(err);
        sqlite3_close(db);
        return;
    }
}

void FinalizeAttributes() {
    sqlite3_close(db);
}

void SetupTileAttributeData(TileData* data) {
    sqlite3_stmt *stmt;
    if (sqlite3_prepare(db, "INSERT INTO tiles(tile_id, ptr) VALUES(?, ?);", -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare insert statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    char* err = 0;
    sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, &err);
    for(int i=0; i < arrlen(data); i++) {
        if (sqlite3_bind_int(stmt, 1, i) != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR: could not bind index value to statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_exec(db, "ROLLBACK TRANSACTION;", 0, 0, &err);
            return;
        }
        if (sqlite3_bind_pointer(stmt, 2, (void*)&data[i], "pTileData", NULL) != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR: could not bind pointer value to statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_exec(db, "ROLLBACK TRANSACTION;", 0, 0, &err);
            return;
        }
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "SQL ERROR: couldn't execute insert statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_exec(db, "ROLLBACK TRANSACTION;", 0, 0, &err);
            return;
        }

        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT TRANSACTION;", 0, 0, &err);
}

long long SetupTileSetAttributeData(TileSet* ts) {
    sqlite3_stmt *stmt;
    if (sqlite3_prepare(db, "INSERT INTO tilesets(ptr) VALUES(?);", -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare insert statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    if (sqlite3_bind_pointer(stmt, 1, (void*)ts, "pTileSetData", NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind pointer value to statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "SQL ERROR: couldn't execute insert statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return sqlite3_last_insert_rowid(db);
}

bool _DoesColumnExist(char* name, AttrType dType) {
    char* query = "";
    if (dType == TILE) {
        query = "SELECT COUNT(*) AS CNTREC FROM pragma_table_info('tiles') WHERE name=?;";
    }
    else if (dType == TILESET) {
        query = "SELECT COUNT(*) AS CNTREC FROM pragma_table_info('tilesets') WHERE name=?;";
    }
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare column check statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    if (sqlite3_bind_text(stmt, 1, name, (int)strlen(name), NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind column name to statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        fprintf(stderr, "SQL ERROR: couldn't execute column check statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    int val = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return val == 1;
}

void _SetAttributeInt(void* data, AttrType dType, char* name, int value) {
    char* tableName = "";
    char* idName = "";
    TileData* tileData = NULL;
    TileSet* tileSetData = NULL;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        tileData = (TileData*)data;
    }
    else if (dType == TILESET) {
        tableName = "tilesets";
        idName = "tileset_id";
        tileSetData = (TileSet*)data;
    }

    sqlite3_stmt* stmt;
    char query[1024];

    if (!_DoesColumnExist(name, dType)) {
        sprintf(query, "ALTER TABLE %s ADD %s INTEGER DEFAULT 0;", tableName, name);
        if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR: could not prepare column add statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return;
        }
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "SQL ERROR: couldn't execute column add statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return;
        }
        sqlite3_finalize(stmt);
    }

    sprintf(query, "UPDATE %s SET %s = ? WHERE %s = ?;", tableName, name, idName);
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare update statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    if (sqlite3_bind_int(stmt, 1, value) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind integer value to update statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    int bindRes = -1;
    if (dType == TILE) {
        bindRes = sqlite3_bind_int64(stmt, 2, (*tileData).i);
    }
    else if (dType == TILESET) {
        bindRes = sqlite3_bind_int64(stmt, 2, (*tileSetData).i);
    }
    if (bindRes != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind index value to update statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "SQL ERROR: couldn't execute integer update statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
}

void SetTileAttributeInt(TileData* data, char* name, int value) {
    _SetAttributeInt((void*)data, TILE, name, value);
}

void SetTileSetAttributeInt(TileSet* data, char* name, int value) {
    _SetAttributeInt((void*)data, TILESET, name, value);
}


void _SetAttributeFloat(void* data, AttrType dType, char* name, float value) {
    char* tableName = "";
    char* idName = "";
    TileData* tileData = NULL;
    TileSet* tileSetData = NULL;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        tileData = (TileData*)data;
    }
    else if (dType == TILESET) {
        tableName = "tilesets";
        idName = "tileset_id";
        tileSetData = (TileSet*)data;
    }

    sqlite3_stmt* stmt;
    char query[1024];

    if (!_DoesColumnExist(name, dType)) {
        sprintf(query, "ALTER TABLE %s ADD %s REAL DEFAULT 0.0;", tableName, name);
        if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR: could not prepare column add statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return;
        }
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "SQL ERROR: couldn't execute column add statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return;
        }
        sqlite3_finalize(stmt);
    }

    sprintf(query, "UPDATE %s SET %s = ? WHERE %s = ?;", tableName, name, idName);
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare update statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    if (sqlite3_bind_double(stmt, 1, (double)value) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind integer value to update statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    int bindRes = -1;
    if (dType == TILE) {
        bindRes = sqlite3_bind_int64(stmt, 2, (*tileData).i);
    }
    else if (dType == TILESET) {
        bindRes = sqlite3_bind_int64(stmt, 2, (*tileSetData).i);
    }
    if (bindRes != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind index value to update statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "SQL ERROR: couldn't execute integer update statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
}

void SetTileAttributeFloat(TileData* data, char* name, float value) {
    _SetAttributeFloat((void*)data, TILE, name, value);
}

void SetTileSetAttributeFloat(TileSet* data, char* name, float value) {
    _SetAttributeFloat((void*)data, TILESET, name, value);
}


void _SetAttributeString(void* data, AttrType dType, char* name, char* value) {
    char* tableName = "";
    char* idName = "";
    TileData* tileData = NULL;
    TileSet* tileSetData = NULL;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        tileData = (TileData*)data;
    }
    else if (dType == TILESET) {
        tableName = "tilesets";
        idName = "tileset_id";
        tileSetData = (TileSet*)data;
    }

    sqlite3_stmt* stmt;
    char query[1024];

    if (!_DoesColumnExist(name, dType)) {
        sprintf(query, "ALTER TABLE %s ADD %s TEXT DEFAULT '';", tableName, name);
        if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR: could not prepare column add statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return;
        }
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "SQL ERROR: couldn't execute column add statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return;
        }
        sqlite3_finalize(stmt);
    }

    sprintf(query, "UPDATE %s SET %s = ? WHERE tile_id = ?;", tableName, name);
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare update statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    if (sqlite3_bind_text(stmt, 1, value, (int)strlen(value), NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind integer value to update statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    int bindRes = -1;
    if (dType == TILE) {
        bindRes = sqlite3_bind_int64(stmt, 2, (*tileData).i);
    }
    else if (dType == TILESET) {
        bindRes = sqlite3_bind_int64(stmt, 2, (*tileSetData).i);
    }
    if (bindRes != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind index value to update statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "SQL ERROR: couldn't execute integer update statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
}

void SetTileAttributeString(TileData* data, char* name, char* value) {
    _SetAttributeString((void*)data, TILE, name, value);
}

void SetTileSetAttributeString(TileSet* data, char* name, char* value) {
    _SetAttributeString((void*)data, TILESET, name, value);
}


int _GetAttributeInt(void* data, AttrType dType, char* name) {
    if (!_DoesColumnExist(name, dType)) {
        return 0;
    }

    char* tableName = "";
    char* idName = "";
    TileData* tileData = NULL;
    TileSet* tileSetData = NULL;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        tileData = (TileData*)data;
    }
    else if (dType == TILESET) {
        tableName = "tilesets";
        idName = "tileset_id";
        tileSetData = (TileSet*)data;
    }

    sqlite3_stmt* stmt;
    char query[1024];

    sprintf(query, "SELECT %s FROM %s WHERE %s = ?;", name, tableName, idName);
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare select statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }
    int bindRes = -1;
    if (dType == TILE) {
        bindRes = sqlite3_bind_int64(stmt, 1, (*tileData).i);
    }
    else if (dType == TILESET) {
        bindRes = sqlite3_bind_int64(stmt, 1, (*tileSetData).i);
    }
    if (bindRes != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind index value to insert statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        fprintf(stderr, "SQL ERROR: couldn't execute select statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    int val = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return val;
}

int GetTileAttributeInt(TileData* data, char* name) {
    return _GetAttributeInt((void*)data, TILE, name);
}

int GetTileSetAttributeInt(TileSet* data, char* name) {
    return _GetAttributeInt((void*)data, TILESET, name);
}


float _GetAttributeFloat(void* data, AttrType dType, char* name) {
    if (!_DoesColumnExist(name, dType)) {
        return 0.0f;
    }

    char* tableName = "";
    char* idName = "";
    TileData* tileData = NULL;
    TileSet* tileSetData = NULL;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        tileData = (TileData*)data;
    }
    else if (dType == TILESET) {
        tableName = "tilesets";
        idName = "tileset_id";
        tileSetData = (TileSet*)data;
    }

    sqlite3_stmt* stmt;
    char query[1024];

    sprintf(query, "SELECT %s FROM %s WHERE %s = ?;", name, tableName, idName);
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare select statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0.0f;
    }
    int bindRes = -1;
    if (dType == TILE) {
        bindRes = sqlite3_bind_int64(stmt, 1, (*tileData).i);
    }
    else if (dType == TILESET) {
        bindRes = sqlite3_bind_int64(stmt, 1, (*tileSetData).i);
    }
    if (bindRes != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind index value to insert statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0.0f;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        fprintf(stderr, "SQL ERROR: couldn't execute select statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0.0f;
    }

    float val = (float)sqlite3_column_double(stmt, 0);
    sqlite3_finalize(stmt);
    return val;
}

float GetTileAttributeFloat(TileData* data, char* name) {
    return _GetAttributeFloat((void*)data, TILE, name);
}

float GetTileSetAttributeFloat(TileSet* data, char* name) {
    return _GetAttributeFloat((void*)data, TILESET, name);
}


char* _GetAttributeString(void* data, AttrType dType, char* name) {
    if (!_DoesColumnExist(name, dType)) {
        return "";
    }

    char* tableName = "";
    char* idName = "";
    TileData* tileData = NULL;
    TileSet* tileSetData = NULL;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        tileData = (TileData*)data;
    }
    else if (dType == TILESET) {
        tableName = "tilesets";
        idName = "tileset_id";
        tileSetData = (TileSet*)data;
    }

    sqlite3_stmt* stmt;
    char query[1024];

    sprintf(query, "SELECT %s FROM %s WHERE %s = ?;", name, tableName, idName);
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare select statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return "";
    }
    int bindRes = -1;
    if (dType == TILE) {
        bindRes = sqlite3_bind_int64(stmt, 1, (*tileData).i);
    }
    else if (dType == TILESET) {
        bindRes = sqlite3_bind_int64(stmt, 1, (*tileSetData).i);
    }
    if (bindRes != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind index value to insert statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return "";
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        fprintf(stderr, "SQL ERROR: couldn't execute select statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return "";
    }

    char* valRaw = (char*)sqlite3_column_text(stmt, 0);
    char* val = malloc(sizeof(char) * strlen(valRaw)+1);
    val = strcpy(val, valRaw);
    sqlite3_finalize(stmt);
    return val;
}

char* GetTileAttributeString(TileData* data, char* name) {
    return _GetAttributeString((void*)data, TILE, name);
}

char* GetTileSetAttributeString(TileSet* data, char* name) {
    return _GetAttributeString((void*)data, TILESET, name);
}


sqlite3_int64 _GetTagID(char* tag) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare(db, "SELECT tag_id FROM tags WHERE value = ?;", -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare tag check statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    if (sqlite3_bind_text(stmt, 1, tag, (int)strlen(tag), NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind tag value to check statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    else {
        sqlite_int64 tagID = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
        return tagID;
    }
}

bool _AddTag(void* data, AttrType dType, char* tag) {
    sqlite3_stmt* stmt;

    sqlite_int64 tagID = _GetTagID(tag);
    if (tagID == -1) {
        // Tag not in DB yet
        if (sqlite3_prepare(db, "INSERT INTO tags(value) VALUES(?);", -1, &stmt, NULL) != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR: could not prepare tag insert statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return false;
        }
        if (sqlite3_bind_text(stmt, 1, tag, (int)strlen(tag), NULL) != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR: could not bind tag value to insert statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return false;
        }
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "SQL ERROR: could not execute tag insert statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return false;
        }
        tagID = sqlite3_last_insert_rowid(db);
        sqlite3_finalize(stmt);
    }

    char* tableName = "";
    char* idName = "";
    TileData* tileData = NULL;
    TileSet* tileSetData = NULL;
    if (dType == TILE) {
        tableName = "tiles_tags";
        idName = "tile_id";
        tileData = (TileData*)data;
    }
    else if (dType == TILESET) {
        tableName = "tilesets_tags";
        idName = "tileset_id";
        tileSetData = (TileSet*)data;
    }

    char query[1024];
    sprintf(query, "INSERT OR IGNORE INTO %s(%s, tag_id) VALUES (?, ?);", tableName, idName);

    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare %s insert statement: %s\n", tableName, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    int bindRes = -1;
    if (dType == TILE) {
        bindRes = sqlite3_bind_int64(stmt, 1, (*tileData).i);
    }
    else if (dType == TILESET) {
        bindRes = sqlite3_bind_int64(stmt, 1, (*tileSetData).i);
    }
    if (bindRes != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind %s to %s insert statement: %s\n", idName, tableName, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    if (sqlite3_bind_int64(stmt, 2, tagID) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind tag_id to %s insert statement: %s\n", tableName, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "SQL ERROR: could not execute %s insert statement: %s\n", tableName, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool AddTileTag(TileData* data, char* tag) {
    return _AddTag((void*)data, TILE, tag);
}

bool AddTileSetTag(TileSet* data, char* tag) {
    return _AddTag((void*)data, TILESET, tag);
}


bool _RemoveTag(void* data, AttrType dType, char* tag) {
    sqlite_int64 tagID = _GetTagID(tag);
    if (tagID == -1) {
        fprintf(stderr, "ERROR: Trying to remove tag '%s' that does not exist.\n", tag);
        return false;
    }

    char* tableName = "";
    char* idName = "";
    TileData* tileData = NULL;
    TileSet* tileSetData = NULL;
    if (dType == TILE) {
        tableName = "tiles_tags";
        idName = "tile_id";
        tileData = (TileData*)data;
    }
    else if (dType == TILESET) {
        tableName = "tilesets_tags";
        idName = "tileset_id";
        tileSetData = (TileSet*)data;
    }

    sqlite3_stmt* stmt;
    char query[1024];
    sprintf(query, "DELETE FROM %s WHERE %s = ? AND tag_id = ?;", tableName, idName);

    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare tag removal statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    int bindRes = -1;
    if (dType == TILE) {
        bindRes = sqlite3_bind_int64(stmt, 1, (*tileData).i);
    }
    else if (dType == TILESET) {
        bindRes = sqlite3_bind_int64(stmt, 1, (*tileSetData).i);
    }
    if (bindRes != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind %s to tag removal statement: %s\n", idName, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    if (sqlite3_bind_int64(stmt, 2, tagID) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind tag_id to tag removal statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "SQL ERROR: could not execute tag removal statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool RemoveTileTag(TileData* data, char* tag) {
    return _RemoveTag(data, TILE, tag);
}

bool RemoveTileSetTag(TileSet* data, char* tag) {
    return _RemoveTag(data, TILESET, tag);
}


void* _GetTagged(AttrType dType, char* tagString) {
    if (strlen(tagString) == 0) {
        return NULL;
    }

    char* tableName = "";
    char* idName = "";
    char* tagTableName = "";
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        tagTableName = "tiles_tags";
    }
    else if (dType == TILESET) {
        tableName = "tilesets";
        idName = "tileset_id";
        tagTableName = "tilesets_tags";
    }

    int tagCount;
    sds* tags = sdssplitlen(tagString, strlen(tagString), ",", 1, &tagCount);

    sds query = sdsempty();
    query = sdscatprintf(query, "SELECT %s.%s ", tableName, idName);
    query = sdscatprintf(query, "FROM %s, %s, tags ", tagTableName, tableName);
    query = sdscatprintf(query, "WHERE %s.tag_id = tags.tag_id ", tagTableName);
    query = sdscat(query, "AND (tags.value IN (?");
    for (int i=0; i < tagCount-1; i++) {
        query = sdscat(query, ", ?");
    }
    query = sdscatprintf(query, ")) AND %s.%s = %s.%s ", tableName, idName, tagTableName, idName);
    query = sdscatprintf(query, "GROUP BY %s.%s ", tableName, idName);
    query = sdscatprintf(query, "HAVING COUNT(%s.%s)=?;", tableName, idName);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare tag search statement: %s\n", sqlite3_errmsg(db));
        sdsfree(query);
        sqlite3_finalize(stmt);
        return NULL;
    }
    sdsfree(query);
    for (int i=0; i < tagCount; i++) {
        if (sqlite3_bind_text(stmt, i+1, tags[i], (int)sdslen(tags[i]), NULL) != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR: could not bind tag '%s' to tag search statement: %s\n", tags[i], sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return NULL;
        }
    }
    if (sqlite3_bind_int(stmt, (int)tagCount+1, (int)tagCount) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind number of tags to tag search statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }

    void* ret = NULL;
    if (dType == TILE) {
        TileData** tdRet = NULL;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            TileData* td = GetTileAtIndex(sqlite3_column_int64(stmt, 0));
            arrpush(tdRet, td);
        }
        ret = (void*)tdRet;
    }
    else if (dType == TILESET) {

    }

    sqlite3_finalize(stmt);
    sdsfreesplitres(tags, tagCount);

    return ret;
}

TileData** GetTilesTagged(char* tagString) {
    return (TileData**)_GetTagged(TILE, tagString);
}

TileSet** GetTileSetsTagged(char* tagString) {
    return (TileSet**)_GetTagged(TILESET, tagString);
}


// this is mostly cut-and-paste from _GetTagged
bool _HasTags(void* data, AttrType dType, char* tagString) {
    if (strlen(tagString) == 0) {
        return false;
    }

    char* tableName = "";
    char* idName = "";
    char* tagTableName = "";
    TileData* tileData = NULL;
    TileSet* tileSetData = NULL;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        tagTableName = "tiles_tags";
        tileData = (TileData*)data;
    }
    else if (dType == TILESET) {
        tableName = "tilesets";
        idName = "tileset_id";
        tagTableName = "tilesets_tags";
        tileSetData = (TileSet*)data;
    }

    int tagCount;
    sds* tags = sdssplitlen(tagString, strlen(tagString), ",", 1, &tagCount);

    sds query = sdsempty();
    query = sdscatprintf(query, "SELECT %s.%s ", tableName, idName);
    query = sdscatprintf(query, "FROM %s, %s, tags ", tagTableName, tableName);
    query = sdscatprintf(query, "WHERE %s.tag_id = tags.tag_id ", tagTableName);
    query = sdscat(query, "AND (tags.value IN (?");
    for (int i=0; i < tagCount-1; i++) {
        query = sdscat(query, ", ?");
    }
    query = sdscatprintf(query, ")) AND %s.%s = %s.%s ", tableName, idName, tagTableName, idName);
    query = sdscatprintf(query, "AND %s.%s = ? ", tableName, idName);
    query = sdscatprintf(query, "GROUP BY %s.%s ", tableName, idName);
    query = sdscatprintf(query, "HAVING COUNT(%s.%s)=?;", tableName, idName);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare tag search statement: %s\n", sqlite3_errmsg(db));
        sdsfree(query);
        sqlite3_finalize(stmt);
        return NULL;
    }
    sdsfree(query);
    for (int i=0; i < tagCount; i++) {
        if (sqlite3_bind_text(stmt, i+1, tags[i], (int)sdslen(tags[i]), NULL) != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR: could not bind tag '%s' to tag search statement: %s\n", tags[i], sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return NULL;
        }
    }
    int bindRes = -1;
    if (dType == TILE) {
        bindRes = sqlite3_bind_int64(stmt, (int)tagCount+1, (*tileData).i);
    }
    else if (dType == TILESET) {
        bindRes = sqlite3_bind_int64(stmt, (int)tagCount+1, (*tileSetData).i);
    }
    if (bindRes != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind index to tag search statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }
    if (sqlite3_bind_int(stmt, (int)tagCount+2, (int)tagCount) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind number of tags to tag search statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }

    bool ret = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ret = true;
    }

    sqlite3_finalize(stmt);
    sdsfreesplitres(tags, tagCount);

    return ret;
}

bool TileHasTags(TileData* data, char* tagString) {
    return _HasTags((void*)data, TILE, tagString);
}

bool TileSetHasTags(TileSet* data, char* tagString) {
    return _HasTags((void*)data, TILESET, tagString);
}


char** _GetTags(void* data, AttrType dType) {
    char* tableName = "";
    char* idName = "";
    char* tagTableName = "";
    TileData* tileData = NULL;
    TileSet* tileSetData = NULL;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        tagTableName = "tiles_tags";
        tileData = (TileData*)data;
    }
    else if (dType == TILESET) {
        tableName = "tilesets";
        idName = "tileset_id";
        tagTableName = "tilesets_tags";
        tileSetData = (TileSet*)data;
    }

    sqlite3_stmt* stmt;

    sds query = sdsnew("SELECT value FROM tags ");
    query = sdscatprintf(query, "JOIN %s ON tags.tag_id = %s.tag_id ", tagTableName, tagTableName);
    query = sdscatprintf(query, "JOIN %s ON %s.%s = %s.%s ", tableName, tableName, idName, tagTableName, idName);
    query = sdscatprintf(query, "WHERE %s.%s = ?;", tableName, idName);

    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare tag get statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }
    int bindRes = -1;
    if (dType == TILE) {
        bindRes = sqlite3_bind_int64(stmt, 1, (*tileData).i);
    }
    else if (dType == TILESET) {
        bindRes = sqlite3_bind_int64(stmt, 1, (*tileSetData).i);
    }
    if (bindRes != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind %s to tag get statement: %s\n", idName, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }

    char** ret = NULL;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char* vp = (char *)sqlite3_column_text(stmt, 0);
        char* v = malloc(sizeof(char) * strlen(vp)+1);
        strcpy(v, vp);
        arrpush(ret, v);
    }

    sqlite3_finalize(stmt);
    return ret;
}

char** GetTileTags(TileData* data) {
    return _GetTags((void*)data, TILE);
}

char** GetTileSetTags(TileSet* data) {
    return _GetTags((void*)data, TILESET);
}

