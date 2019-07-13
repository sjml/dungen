#include "../stdafx.h"
#include "attributes.h"

#include <sqlite3.h>

sqlite3* db;

void InitializeAttributes() {
    const char* dbPath = ":memory:";
    int ret = sqlite3_open(dbPath, &db);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    
    char* err = 0;
    char *creation =
        "CREATE TABLE tiles(id INTEGER PRIMARY KEY, ptr INTEGER);"
        "CREATE TABLE tags(id INTEGER PRIMARY KEY, value STRING);"
        "CREATE TABLE tiles_tags(tile_id INTEGER, tag_id INTEGER,"
            "FOREIGN KEY(tile_id) REFERENCES tiles(id),"
            "FOREIGN KEY(tag_id) REFERENCES tags(id)"
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
    if (sqlite3_prepare(db, "INSERT INTO tiles(id, ptr) VALUES(?, ?);", -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare insert statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    for(int i=0; i < arrlen(data); i++) {
        if (sqlite3_bind_int(stmt, 1, i) != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR: could not bind index value to statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return;
        }
        if (sqlite3_bind_pointer(stmt, 2, (void*)&data[i], "pTileData", NULL) != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR: could not bind index value to statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return;
        }
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "SQL ERROR: couldn't execute insert statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return;
        }
        
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }
    
    sqlite3_finalize(stmt);
}
