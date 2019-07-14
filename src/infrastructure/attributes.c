#include "../stdafx.h"
#include "attributes.h"

// Doing all this with SQLite may be... overkill?
//     Using a sledgehammer to crack a walnut?
//     Whatever. It's done. Here it is.
#include <sqlite3.h>

sqlite3* db;

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
        "CREATE TABLE tags(tag_id INTEGER PRIMARY KEY, value STRING UNIQUE);"
        "CREATE INDEX tagIdx ON tags(value);"
        "CREATE TABLE tiles_tags(tile_id INTEGER, tag_id INTEGER, "
            "FOREIGN KEY(tile_id) REFERENCES tiles(tile_id), "
            "FOREIGN KEY(tag_id) REFERENCES tags(tag_id), "
            "PRIMARY KEY(tile_id, tag_id)"
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

void SetupAttributeData(TileData* data) {
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
            fprintf(stderr, "SQL ERROR: could not bind index value to statement: %s\n", sqlite3_errmsg(db));
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

bool _DoesColumnExist(char* name) {
    sqlite3_stmt* stmt;
    if (sqlite3_prepare(db, "SELECT COUNT(*) AS CNTREC FROM pragma_table_info('tiles') WHERE name=?;", -1, &stmt, NULL) != SQLITE_OK) {
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

void SetAttributeInt(TileData* data, char* name, int value) {
    sqlite3_stmt* stmt;
    char query[1024];

    if (!_DoesColumnExist(name)) {
        sprintf(query, "ALTER TABLE tiles ADD %s INTEGER DEFAULT 0;", name);
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

    sprintf(query, "UPDATE tiles SET %s = ? WHERE tile_id = ?;", name);
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
    if (sqlite3_bind_int64(stmt, 2, (*data).i) != SQLITE_OK) {
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

void SetAttributeFloat(TileData* data, char* name, float value) {
    sqlite3_stmt* stmt;
    char query[1024];

    if (!_DoesColumnExist(name)) {
        sprintf(query, "ALTER TABLE tiles ADD %s REAL DEFAULT 0.0;", name);
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

    sprintf(query, "UPDATE tiles SET %s = ? WHERE tile_id = ?;", name);
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
    if (sqlite3_bind_int64(stmt, 2, (*data).i) != SQLITE_OK) {
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

void SetAttributeString(TileData* data, char* name, char* value) {
    sqlite3_stmt* stmt;
    char query[1024];

    if (!_DoesColumnExist(name)) {
        sprintf(query, "ALTER TABLE tiles ADD %s TEXT DEFAULT '';", name);
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

    sprintf(query, "UPDATE tiles SET %s = ? WHERE tile_id = ?;", name);
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
    if (sqlite3_bind_int64(stmt, 2, (*data).i) != SQLITE_OK) {
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

int GetAttributeInt(TileData* data, char* name) {
    if (!_DoesColumnExist(name)) {
        return 0;
    }

    sqlite3_stmt* stmt;
    char query[1024];

    sprintf(query, "SELECT %s FROM tiles WHERE tile_id = ?;", name);
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare select statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }
    if (sqlite3_bind_int64(stmt, 1, (*data).i) != SQLITE_OK) {
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

float GetAttributeFloat(TileData* data, char* name) {
    if (!_DoesColumnExist(name)) {
        return 0.0f;
    }

    sqlite3_stmt* stmt;
    char query[1024];

    sprintf(query, "SELECT %s FROM tiles WHERE tile_id = ?;", name);
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare select statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0.0f;
    }
    if (sqlite3_bind_int64(stmt, 1, (*data).i) != SQLITE_OK) {
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

char* GetAttributeString(TileData* data, char* name) {
    if (!_DoesColumnExist(name)) {
        return "";
    }

    sqlite3_stmt* stmt;
    char query[1024];

    sprintf(query, "SELECT %s FROM tiles WHERE tile_id = ?;", name);
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare select statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return "";
    }
    if (sqlite3_bind_int64(stmt, 1, (*data).i) != SQLITE_OK) {
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

bool AddTag(TileData* td, char* tag) {
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
    
    if (sqlite3_prepare(db, "INSERT OR IGNORE INTO tiles_tags(tile_id, tag_id) VALUES (?, ?);", -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare tag_tile insert statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    if (sqlite3_bind_int64(stmt, 1, (*td).i) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind tile_id to tag_tile insert statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    if (sqlite3_bind_int64(stmt, 2, tagID) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind tag_id to tag_tile insert statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "SQL ERROR: could not execute tag_tile insert statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    
    sqlite3_finalize(stmt);
    return true;
}

bool RemoveTag(TileData* td, char* tag) {
    sqlite_int64 tagID = _GetTagID(tag);
    if (tagID == -1) {
        fprintf(stderr, "ERROR: Trying to remove tag '%s' that does not exist.\n", tag);
        return false;
    }

    sqlite3_stmt* stmt;
    if (sqlite3_prepare(db, "DELETE FROM tiles_tags WHERE tile_id = ? AND tag_id = ?;", -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare tag removal statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    if (sqlite3_bind_int64(stmt, 1, (*td).i) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind tile_id to tag removal statement: %s\n", sqlite3_errmsg(db));
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

TileData** GetTilesTagged(char* tagString) {
    if (strlen(tagString) == 0) {
        return NULL;
    }
    
    char** tags = NULL;
    char* tag;
    char* localTagString = malloc(sizeof(char) * strlen(tagString)+1);
    strcpy(localTagString, tagString);
    while ((tag = strtok(localTagString, ","))) {
        char* localTag = malloc(sizeof(char) * strlen(tag)+1);
        strcpy(localTag, tag);
        arrpush(tags, localTag);
        localTagString = NULL;
    }
    free(localTagString);
    
    sds query = sdsnew(
        "SELECT tiles.tile_id "
        "FROM tiles_tags, tiles, tags "
        "WHERE tiles_tags.tag_id = tags.tag_id "
        "AND (tags.value IN (?"
    );
    for (int i=0; i < arrlen(tags)-1; i++) {
        query = sdscat(query, ", ?");
    }
    query = sdscat(query,
        ")) "
        "AND tiles.tile_id = tiles_tags.tile_id "
        "GROUP BY tiles.tile_id "
        "HAVING COUNT(tiles.tile_id)=?;"
    );
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare tag search statement: %s\n", sqlite3_errmsg(db));
        sdsfree(query);
        sqlite3_finalize(stmt);
        return NULL;
    }
    sdsfree(query);
    for (int i=0; i < arrlen(tags); i++) {
        if (sqlite3_bind_text(stmt, i+1, tags[i], (int)strlen(tags[i]), NULL) != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR: could not bind tag '%s' to tag search statement: %s\n", tags[i], sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return NULL;
        }
    }
    if (sqlite3_bind_int(stmt, (int)arrlen(tags)+1, (int)arrlen(tags)) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind number of tags to tag search statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }
    
    TileData** ret = NULL;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TileData* td = GetTileAtIndex(sqlite3_column_int64(stmt, 0));
        arrpush(ret, td);
    }
    
    sqlite3_finalize(stmt);
    
    for (int i=0; i < arrlen(tags); i++) {
        free(tags[i]);
    }
    arrfree(tags);
    
    return ret;
}

char** GetTags(TileData* td) {
    sqlite3_stmt* stmt;
    
    const char* query =
        "SELECT value FROM tags "
        "JOIN tiles_tags ON tags.tag_id = tiles_tags.tag_id "
        "JOIN tiles ON tiles.tile_id = tiles_tags.tile_id "
        "WHERE tiles.tile_id = ?;";
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare tag get statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }
    if (sqlite3_bind_int64(stmt, 1, (*td).i) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind tile_id to tag get statement: %s\n", sqlite3_errmsg(db));
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
