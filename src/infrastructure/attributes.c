#include "stdafx.h"
#include "attributes.h"

#include "rendering.h"

static long long __tagIdx = 0;
static struct { char* key; long long value; } *tags_StringToIdx = NULL;
static struct { long long key; char* value; } *tags_IdxToString = NULL;

static struct { long long key; long long* value; } *tagIdxToTiles = NULL;
static struct { long long key; long long* value; } *tileIdxToTags = NULL;

static struct { long long key; long long* value; } *tagIdxToRegions = NULL;
static struct { long long key; long long* value; } *regionIdxToTags = NULL;

static struct { long long key; long long* value; } *tagIdxToAgents = NULL;
static struct { long long key; long long* value; } *agentIdxToTags = NULL;


// Doing all this with SQLite may be... overkill?
//     Using a sledgehammer to crack a walnut?
//     Whatever. It's done. Here it is.
#include <sqlite3.h>

static sqlite3* db;

typedef enum eAttrType {
    TILE,
    REGION,
    AGENT
} AttrType;

void InitializeAttributes() {
    shdefault(tags_StringToIdx, -1);
    hmdefault(tags_IdxToString, "");
    hmdefault(tagIdxToTiles, NULL);
    hmdefault(tagIdxToRegions, NULL);
    hmdefault(tileIdxToTags, NULL);
    hmdefault(regionIdxToTags, NULL);

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
        "CREATE TABLE regions(region_id INTEGER PRIMARY KEY, ptr INTEGER);"
        "CREATE TABLE agents(agent_id INTEGER PRIMARY KEY, ptr INTEGER);"
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

long long SetupRegionAttributeData(Region* r) {
    sqlite3_stmt *stmt;
    if (sqlite3_prepare(db, "INSERT INTO regions(ptr) VALUES(?);", -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare insert statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    if (sqlite3_bind_pointer(stmt, 1, (void*)r, "pRegion", NULL) != SQLITE_OK) {
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

void ClearRegionAttributeData(Region* r) {
    sqlite3_stmt *stmt;
    if (sqlite3_prepare(db, "DELETE FROM regions WHERE region_id = ?;", -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare deletion statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    if (sqlite3_bind_int64(stmt, 1, r->i) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind index value to deletion statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "SQL ERROR: couldn't execute deletion statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);
}

long long SetupAgentAttributeData(Agent* a) {
    sqlite3_stmt *stmt;
    if (sqlite3_prepare(db, "INSERT INTO agents(agent_id, ptr) VALUES(?, ?);", -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare insert statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    if (sqlite3_bind_int64(stmt, 1, a->i) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind index value to statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    if (sqlite3_bind_pointer(stmt, 2, (void*)a, "pAgent", NULL) != SQLITE_OK) {
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

bool _DoesColumnExist(const char* name, AttrType dType) {
    char* query = "";
    if (dType == TILE) {
        query = "SELECT COUNT(*) AS CNTREC FROM pragma_table_info('tiles') WHERE name=?;";
    }
    else if (dType == REGION) {
        query = "SELECT COUNT(*) AS CNTREC FROM pragma_table_info('regions') WHERE name=?;";
    }
    else if (dType == AGENT) {
        query = "SELECT COUNT(*) AS CNTREC FROM pragma_table_info('agents') WHERE name=?;";
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

void _SetAttributeInt(void* data, AttrType dType, const char* name, int value) {
    char* tableName = "";
    char* idName = "";
    long long idx = -1;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        idx = ((TileData*)data)->meta->i;
    }
    else if (dType == REGION) {
        tableName = "regions";
        idName = "region_id";
        idx = ((Region*)data)->i;
    }
    else if (dType == AGENT) {
        tableName = "agents";
        idName = "agent_id";
        idx = ((Agent*)data)->i;
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
    if (sqlite3_bind_int64(stmt, 2, idx) != SQLITE_OK) {
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

void SetTileAttributeInt(TileData* data, const char* name, int value) {
    _SetAttributeInt((void*)data, TILE, name, value);
    SetTileAsDirty(data);
}

void SetRegionAttributeInt(Region* data, const char* name, int value) {
    _SetAttributeInt((void*)data, REGION, name, value);
    SetRegionAsDirty(data);
}

void SetAgentAttributeInt(Agent* data, const char* name, int value) {
    _SetAttributeInt((void*)data, AGENT, name, value);
}


void _SetAttributeFloat(void* data, AttrType dType, const char* name, float value) {
    char* tableName = "";
    char* idName = "";
    long long idx = -1;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        idx = ((TileData*)data)->meta->i;
    }
    else if (dType == REGION) {
        tableName = "regions";
        idName = "region_id";
        idx = ((Region*)data)->i;
    }
    else if (dType == AGENT) {
        tableName = "agents";
        idName = "agent_id";
        idx = ((Agent*)data)->i;
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

    if (sqlite3_bind_int64(stmt, 2, idx) != SQLITE_OK) {
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

void SetTileAttributeFloat(TileData* data, const char* name, float value) {
    _SetAttributeFloat((void*)data, TILE, name, value);
    SetTileAsDirty(data);
}

void SetRegionAttributeFloat(Region* data, const char* name, float value) {
    _SetAttributeFloat((void*)data, REGION, name, value);
    SetRegionAsDirty(data);
}

void SetAgentAttributeFloat(Agent* data, const char* name, float value) {
    _SetAttributeFloat((void*)data, AGENT, name, value);
}


void _SetAttributeString(void* data, AttrType dType, const char* name, const char* value) {
    char* tableName = "";
    char* idName = "";
    long long idx = -1;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        idx = ((TileData*)data)->meta->i;
    }
    else if (dType == REGION) {
        tableName = "regions";
        idName = "region_id";
        idx = ((Region*)data)->i;
    }
    else if (dType == AGENT) {
        tableName = "agents";
        idName = "agent_id";
        idx = ((Agent*)data)->i;
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

    sprintf(query, "UPDATE %s SET %s = ? WHERE %s = ?;", tableName, name, idName);
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

    if (sqlite3_bind_int64(stmt, 2, idx) != SQLITE_OK) {
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

void SetTileAttributeString(TileData* data, const char* name, const char* value) {
    _SetAttributeString((void*)data, TILE, name, value);
    SetTileAsDirty(data);
}

void SetRegionAttributeString(Region* data, const char* name, const char* value) {
    _SetAttributeString((void*)data, REGION, name, value);
    SetRegionAsDirty(data);
}

void SetAgentAttributeString(Agent* data, const char* name, const char* value) {
    _SetAttributeString((void*)data, AGENT, name, value);
}


int _GetAttributeInt(void* data, AttrType dType, const char* name) {
    if (!_DoesColumnExist(name, dType)) {
        return 0;
    }

    char* tableName = "";
    char* idName = "";
    long long idx = -1;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        idx = ((TileData*)data)->meta->i;
    }
    else if (dType == REGION) {
        tableName = "regions";
        idName = "region_id";
        idx = ((Region*)data)->i;
    }
    else if (dType == AGENT) {
        tableName = "agents";
        idName = "agent_id";
        idx = ((Agent*)data)->i;
    }

    sqlite3_stmt* stmt;
    char query[1024];

    sprintf(query, "SELECT %s FROM %s WHERE %s = ?;", name, tableName, idName);
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare select statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    if (sqlite3_bind_int64(stmt, 1, idx) != SQLITE_OK) {
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

int GetTileAttributeInt(TileData* data, const char* name) {
    return _GetAttributeInt((void*)data, TILE, name);
}

int GetRegionAttributeInt(Region* data, const char* name) {
    return _GetAttributeInt((void*)data, REGION, name);
}

int GetAgentAttributeInt(Agent* data, const char* name) {
    return _GetAttributeInt((void*)data, AGENT, name);
}


float _GetAttributeFloat(void* data, AttrType dType, const char* name) {
    if (!_DoesColumnExist(name, dType)) {
        return 0.0f;
    }

    char* tableName = "";
    char* idName = "";
    long long idx = -1;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        idx = ((TileData*)data)->meta->i;
    }
    else if (dType == REGION) {
        tableName = "regions";
        idName = "region_id";
        idx = ((Region*)data)->i;
    }
    else if (dType == AGENT) {
        tableName = "agents";
        idName = "agent_id";
        idx = ((Agent*)data)->i;
    }

    sqlite3_stmt* stmt;
    char query[1024];

    sprintf(query, "SELECT %s FROM %s WHERE %s = ?;", name, tableName, idName);
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare select statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0.0f;
    }

    if (sqlite3_bind_int64(stmt, 1, idx) != SQLITE_OK) {
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

float GetTileAttributeFloat(TileData* data, const char* name) {
    return _GetAttributeFloat((void*)data, TILE, name);
}

float GetRegionAttributeFloat(Region* data, const char* name) {
    return _GetAttributeFloat((void*)data, REGION, name);
}

float GetAgentAttributeFloat(Agent* data, const char* name) {
    return _GetAttributeFloat((void*)data, AGENT, name);
}


char* _GetAttributeString(void* data, AttrType dType, const char* name) {
    if (!_DoesColumnExist(name, dType)) {
        return "";
    }

    char* tableName = "";
    char* idName = "";
    long long idx = -1;
    if (dType == TILE) {
        tableName = "tiles";
        idName = "tile_id";
        idx = ((TileData*)data)->meta->i;
    }
    else if (dType == REGION) {
        tableName = "regions";
        idName = "region_id";
        idx = ((Region*)data)->i;
    }
    else if (dType == AGENT) {
        tableName = "agents";
        idName = "agent_id";
        idx = ((Agent*)data)->i;
    }

    sqlite3_stmt* stmt;
    char query[1024];

    sprintf(query, "SELECT %s FROM %s WHERE %s = ?;", name, tableName, idName);
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not prepare select statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return "";
    }

    if (sqlite3_bind_int64(stmt, 1, idx) != SQLITE_OK) {
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

char* GetTileAttributeString(TileData* data, const char* name) {
    return _GetAttributeString((void*)data, TILE, name);
}

char* GetRegionAttributeString(Region* data, const char* name) {
    return _GetAttributeString((void*)data, REGION, name);
}

char* GetAgentAttributeString(Agent* data, const char* name) {
    return _GetAttributeString((void*)data, AGENT, name);
}


long long _GetTagID(const char* tag) {
    return shget(tags_StringToIdx, tag);
}

bool AddTileTag(TileData* data, char* tag) {
    long long id = _GetTagID(tag);
    if (id == -1) {
        hmput(tags_IdxToString, __tagIdx, tag);
        shput(tags_StringToIdx, tag, __tagIdx);
        id = __tagIdx;
        __tagIdx++;
    }
    long long* tileList = hmget(tagIdxToTiles, id);
    bool already = false;
    for (int i=0; i < arrlen(tileList); i++) {
        if (tileList[i] == data->meta->i) {
            already = true;
            break;
        }
    }
    if (already) {
        return false;
    }
    arrpush(tileList, data->meta->i);
    hmput(tagIdxToTiles, id, tileList);

    long long* tagList = hmget(tileIdxToTags, data->meta->i);
    arrpush(tagList, id);
    hmput(tileIdxToTags, data->meta->i, tagList);

    SetTileAsDirty(data);
    return true;
}

bool AddRegionTag(Region* data, char* tag) {
    long long id = _GetTagID(tag);
    if (id == -1) {
        hmput(tags_IdxToString, __tagIdx, tag);
        shput(tags_StringToIdx, tag, __tagIdx);
        id = __tagIdx;
        __tagIdx++;
    }
    long long* regionList = hmget(tagIdxToRegions, id);
    bool already = false;
    for (int i=0; i < arrlen(regionList); i++) {
        if (regionList[i] == data->i) {
            already = true;
            break;
        }
    }
    if (already) {
        return false;
    }
    arrpush(regionList, data->i);
    hmput(tagIdxToRegions, id, regionList);

    long long* tagList = hmget(regionIdxToTags, data->i);
    arrpush(tagList, id);
    hmput(regionIdxToTags, data->i, tagList);

    SetRegionAsDirty(data);
    return true;
}

bool AddAgentTag(Agent* data, char* tag) {
    long long id = _GetTagID(tag);
    if (id == -1) {
        hmput(tags_IdxToString, __tagIdx, tag);
        shput(tags_StringToIdx, tag, __tagIdx);
        id = __tagIdx;
        __tagIdx++;
    }
    long long* agentList = hmget(tagIdxToAgents, id);
    bool already = false;
    for (int i=0; i < arrlen(agentList); i++) {
        if (agentList[i] == data->i) {
            already = true;
            break;
        }
    }
    if (already) {
        return false;
    }
    arrpush(agentList, data->i);
    hmput(tagIdxToAgents, id, agentList);

    long long* tagList = hmget(agentIdxToTags, data->i);
    arrpush(tagList, id);
    hmput(agentIdxToTags, data->i, tagList);

    return true;
}

bool RemoveTileTag(TileData* data, const char* tag) {
    long long id = _GetTagID(tag);
    if (id == -1) {
        return false;
    }
    long long* tileList = hmget(tagIdxToTiles, id);
    for (int i = 0; i < arrlen(tileList); i++) {
        if (tileList[i] == data->meta->i) {
            long long* tagList = hmget(tileIdxToTags, data->meta->i);
            for (int j = 0; j < arrlen(tagList); j++) {
                if (tagList[j] == id) {
                    arrdel(tagList, j);
                    hmput(tileIdxToTags, data->meta->i, tagList);
                    break;
                }
            }
            arrdel(tileList, i);
            hmput(tagIdxToTiles, id, tileList);
            SetTileAsDirty(data);
            return true;
        }
    }
    return false;
}

bool RemoveRegionTag(Region* data, const char* tag) {
    long long id = _GetTagID(tag);
    if (id == -1) {
        return false;
    }
    long long* regionList = hmget(tagIdxToRegions, id);
    for (int i = 0; i < arrlen(regionList); i++) {
        if (regionList[i] == data->i) {
            long long* tagList = hmget(regionIdxToTags, data->i);
            for (int j = 0; j < arrlen(tagList); j++) {
                if (tagList[j] == id) {
                    arrdel(tagList, j);
                    hmput(regionIdxToTags, data->i, tagList);
                    break;
                }
            }
            arrdel(regionList, i);
            hmput(tagIdxToRegions, id, regionList);
            SetRegionAsDirty(data);
            return true;
        }
    }
    return false;
}

bool RemoveAgentTag(Agent* data, const char* tag) {
    long long id = _GetTagID(tag);
    if (id == -1) {
        return false;
    }
    long long* agentList = hmget(tagIdxToAgents, id);
    for (int i = 0; i < arrlen(agentList); i++) {
        if (agentList[i] == data->i) {
            long long* tagList = hmget(agentIdxToTags, data->i);
            for (int j = 0; j < arrlen(tagList); j++) {
                if (tagList[j] == id) {
                    arrdel(tagList, j);
                    hmput(agentIdxToTags, data->i, tagList);
                    break;
                }
            }
            arrdel(agentList, i);
            hmput(tagIdxToAgents, id, agentList);
            return true;
        }
    }
    return false;
}

sds* _TagSplit(const char* tagString, int* tagCount) {
    sds* tags = sdssplitlen(tagString, strlen(tagString), ",", 1, tagCount);

    for (int i = 0; i < *tagCount; i++) {
        tags[i] = sdstrim(tags[i], " ");
    }

    return tags;
}

TileData** GetTilesTagged(const char* tagString) {
    int tagCount;
    sds* tags = _TagSplit(tagString, &tagCount);

    TileData** ret = NULL;
    long long* indices = NULL;

    for (int ti = 0; ti < tagCount; ti++) {
        long long id = _GetTagID(tags[ti]);
        if (id == -1) {
            ret = NULL;
            break;
        }
        long long* tiles = hmget(tagIdxToTiles, id);
        if (ti == 0) {
            // first tag, add them all
            for (int tf=0; tf < arrlen(tiles); tf++) {
                arrpush(indices, tiles[tf]);
            }
        }
        else {
            // subsequent tags; remove from indices if they don't have id
            for (int fi=0; fi < arrlen(indices); fi++) {
                bool found = false;
                for (int tf=0; tf < arrlen(tiles); tf++) {
                    if (indices[fi] == tiles[tf]) {
                        // tile is in both; we're good
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    arrdel(indices, fi);
                }
            }
        }
    }

    for (int i=0; i < arrlen(indices); i++) {
        arrpush(ret, GetTileAtIndex(indices[i]));
    }

    sdsfreesplitres(tags, tagCount);
    arrfree(indices);
    return ret;
}

DisposableRegionList GetRegionsTagged(const char* tagString) {
    int tagCount;
    sds* tags = _TagSplit(tagString, &tagCount);

    Region** ret = NULL;
    long long* indices = NULL;

    for (int ti = 0; ti < tagCount; ti++) {
        long long id = _GetTagID(tags[ti]);
        if (id == -1) {
            ret = NULL;
            break;
        }
        long long* regions = hmget(tagIdxToRegions, id);
        if (ti == 0) {
            // first tag, add them all
            for (int tsf=0; tsf < arrlen(regions); tsf++) {
                arrpush(indices, regions[tsf]);
            }
        }
        else {
            // subsequent tags; remove from indices if they don't have id
            for (int fi=0; fi < arrlen(indices); fi++) {
                bool found = false;
                for (int tsf=0; tsf < arrlen(regions); tsf++) {
                    if (indices[fi] == regions[tsf]) {
                        // region is in both; we're good
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    arrdel(indices, fi);
                }
            }
        }
    }

    // <sigh> couldn't get the pointer to pull out from the SQLite db, so we're stuck
    //    doing it this way. Inefficient AF, but this function is called rarely, so eh.
    Region** tss = GetRenderingRegions();
    for (int i=0; i < arrlen(indices); i++) {
        for (int tsi=0; tsi < arrlen(tss); tsi++) {
            if (indices[i] == tss[tsi]->i) {
                arrpush(ret, tss[tsi]);
                break;
            }
        }
    }

    sdsfreesplitres(tags, tagCount);
    arrfree(indices);
    return ret;
}

Agent** GetAgentsTagged(const char* tagString) {
    int tagCount;
    sds* tags = _TagSplit(tagString, &tagCount);

    Agent** ret = NULL;
    long long* indices = NULL;

    for (int ti = 0; ti < tagCount; ti++) {
        long long id = _GetTagID(tags[ti]);
        if (id == -1) {
            ret = NULL;
            break;
        }
        long long* agents = hmget(tagIdxToAgents, id);
        if (ti == 0) {
            // first tag, add them all
            for (int tf=0; tf < arrlen(agents); tf++) {
                arrpush(indices, agents[tf]);
            }
        }
        else {
            // subsequent tags; remove from indices if they don't have id
            for (int fi=0; fi < arrlen(indices); fi++) {
                bool found = false;
                for (int tf=0; tf < arrlen(agents); tf++) {
                    if (indices[fi] == agents[tf]) {
                        // agent is in both; we're good
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    arrdel(indices, fi);
                }
            }
        }
    }

    Agent** allAgents = GetAllAgents();
    for (int i=0; i < arrlen(indices); i++) {
        arrpush(ret, allAgents[i]);
    }

    sdsfreesplitres(tags, tagCount);
    arrfree(indices);
    return ret;
}

bool TileHasTags(TileData* data, const char* tagString) {
    int tagCount;
    sds* tags = _TagSplit(tagString, &tagCount);

    bool ret = true;
    for (int ti = 0; ti < tagCount; ti++) {
        long long id = _GetTagID(tags[ti]);
        if (id == -1) {
            ret = false;
            break;
        }
        long long* tiles = hmget(tagIdxToTiles, id);
        bool found = false;
        for (int t=0; t < arrlen(tiles); t++) {
            if (tiles[t] == data->meta->i) {
                found = true;
                break;
            }
        }
        if (!found) {
            ret = false;
            break;
        }
    }

    sdsfreesplitres(tags, tagCount);
    return ret;
}

bool RegionHasTags(Region* data, const char* tagString) {
    int tagCount;
    sds* tags = _TagSplit(tagString, &tagCount);

    bool ret = true;
    for (int ti = 0; ti < tagCount; ti++) {
        long long id = _GetTagID(tags[ti]);
        if (id == -1) {
            ret = false;
            break;
        }
        long long* regions = hmget(tagIdxToRegions, id);
        bool found = false;
        for (int ts=0; ts < arrlen(regions); ts++) {
            if (regions[ts] == data->i) {
                found = true;
                break;
            }
        }
        if (!found) {
            ret = false;
            break;
        }
    }

    sdsfreesplitres(tags, tagCount);
    return ret;
}

bool AgentHasTags(Agent* data, const char* tagString) {
    int tagCount;
    sds* tags = _TagSplit(tagString, &tagCount);

    bool ret = true;
    for (int ti = 0; ti < tagCount; ti++) {
        long long id = _GetTagID(tags[ti]);
        if (id == -1) {
            ret = false;
            break;
        }
        long long* agents = hmget(tagIdxToAgents, id);
        bool found = false;
        for (int ts=0; ts < arrlen(agents); ts++) {
            if (agents[ts] == data->i) {
                found = true;
                break;
            }
        }
        if (!found) {
            ret = false;
            break;
        }
    }

    sdsfreesplitres(tags, tagCount);
    return ret;
}

char** GetTileTags(TileData* data) {
    char** ret = NULL;
    long long* tagIndices = hmget(tileIdxToTags, data->meta->i);

    for (int i=0; i < arrlen(tagIndices); i++) {
        char* t = hmget(tags_IdxToString, tagIndices[i]);
        char* tc = malloc(sizeof(char) * (strlen(t)+1));
        strcpy(tc, t);
        arrpush(ret, tc);
    }
    return ret;
}

char** GetRegionTags(Region* data) {
    char** ret = NULL;
    long long* tagIndices = hmget(regionIdxToTags, data->i);

    for (int i=0; i < arrlen(tagIndices); i++) {
        char* t = hmget(tags_IdxToString, tagIndices[i]);
        char* tc = malloc(sizeof(char) * (strlen(t)+1));
        strcpy(tc, t);
        arrpush(ret, tc);
    }
    return ret;
}

char** GetAgentTags(Agent* data) {
    char** ret = NULL;
    long long* tagIndices = hmget(agentIdxToTags, data->i);

    for (int i=0; i < arrlen(tagIndices); i++) {
        char* t = hmget(tags_IdxToString, tagIndices[i]);
        char* tc = malloc(sizeof(char) * (strlen(t)+1));
        strcpy(tc, t);
        arrpush(ret, tc);
    }
    return ret;
}

void** _GetByAttribute(AttrType dType, const char* attrName, AttrComparison comp, const char* value) {
    void** ret = NULL;

    sds query = sdsnew("SELECT ");
    if (dType == TILE) {
        query = sdscat(query, "tile_id FROM tiles WHERE ");
    }
    else if (dType == REGION) {
        query = sdscat(query, "region_id FROM regions WHERE ");
    }
    else if (dType == AGENT) {
        query = sdscat(query, "agent_id FROM agents WHERE ");
    }

    query = sdscat(query, attrName);
    switch (comp) {
        case LessThan:
            query = sdscat(query, " < \"");
            break;
        case LessThanOrEqual:
            query = sdscat(query, " <= \"");
            break;
        case GreaterThan:
            query = sdscat(query, " > \"");
            break;
        case GreaterThanOrEqual:
            query = sdscat(query, " >= \"");
            break;
        case Equal:
            query = sdscat(query, " = \"");
            break;
        case NotEqual:
            query = sdscat(query, " != \"");
            break;
    }
    query = sdscat(query, value);
    query = sdscat(query, "\";");

    sqlite3_stmt* stmt;
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL WARNING: could not prepare attribute selection statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sdsfree(query);
        return ret;
    }
    sdsfree(query);

    while(sqlite3_step(stmt) == SQLITE_ROW) {
        long long idx = sqlite3_column_int64(stmt, 0);
        void* retData = NULL;
        if (dType == TILE) {
            retData = (void*)GetTileAtIndex(idx);
        }
        else if (dType == REGION) {
            Region** rs = GetRenderingRegions();
            for (long i=0; i < arrlen(rs); i++) {
                if (rs[i]->i == idx) {
                    retData = (void*)rs[i];
                    break;
                }
            }
        }
        else if (dType == AGENT) {
            Agent** allAgents = GetAllAgents();
            retData = (void*)allAgents[idx];
        }
        if (retData != NULL) {
            arrpush(ret, retData);
        }
        else {
            fprintf(stderr, "SQL ERROR: Returning ids of non-existent tiles?\n");
        }
    }

    sqlite3_finalize(stmt);
    return ret;
}

TileData** GetTilesByAttribute(const char* attrName, AttrComparison comp, const char* value) {
    TileData** ret = (TileData**)_GetByAttribute(TILE, attrName, comp, value);
    return ret;
}
DisposableRegionList GetRegionsByAttribute(const char* attrName, AttrComparison comp, const char* value) {
    Region** ret = (Region**)_GetByAttribute(REGION, attrName, comp, value);
    return ret;
}
Agent** GetAgentsByAttribute(const char* attrName, AttrComparison comp, const char* value) {
    Agent** ret = (Agent**)_GetByAttribute(AGENT, attrName, comp, value);
    return ret;
}

bool _CheckAttribute(void* data, AttrType dType, const char* attrName, AttrComparison comp, const char* value) {
    sds query = sdsnew("SELECT COUNT(*) FROM ");
    long long idx = -1;
    if (dType == TILE) {
        query = sdscat(query, "tiles WHERE tile_id = ? AND ");
        idx = ((TileData*)data)->meta->i;
    }
    else if (dType == REGION) {
        query = sdscat(query, "regions WHERE region_id = ? AND ");
        idx = ((Region*)data)->i;
    }
    else if (dType == AGENT) {
        query = sdscat(query, "agents WHERE agent_id = ? AND ");
        idx = ((Agent*)data)->i;
    }
    query = sdscat(query, attrName);
    switch (comp) {
        case LessThan:
            query = sdscat(query, " < \"");
            break;
        case LessThanOrEqual:
            query = sdscat(query, " <= \"");
            break;
        case GreaterThan:
            query = sdscat(query, " > \"");
            break;
        case GreaterThanOrEqual:
            query = sdscat(query, " >= \"");
            break;
        case Equal:
            query = sdscat(query, " = \"");
            break;
        case NotEqual:
            query = sdscat(query, " != \"");
            break;
    }
    query = sdscat(query, value);
    query = sdscat(query, "\";");

    sqlite3_stmt* stmt;
    if (sqlite3_prepare(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL WARNING: could not prepare attribute check statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sdsfree(query);
        return false;
    }
    sdsfree(query);

    if (sqlite3_bind_int64(stmt, 1, idx) != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: could not bind index value to attribute check statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        fprintf(stderr, "SQL ERROR: couldn't execute attribute check statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    int val = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return val > 0;
}

bool CheckTileAttribute(TileData* td, const char* attrName, AttrComparison comp, const char* value) {
    return _CheckAttribute((void*)td, TILE, attrName, comp, value);
}
bool CheckRegionAttribute(Region* r, const char* attrName, AttrComparison comp, const char* value) {
    return _CheckAttribute((void*)r, REGION, attrName, comp, value);
}
bool CheckAgentAttribute(Agent* a, const char* attrName, AttrComparison comp, const char* value) {
    return _CheckAttribute((void*)a, AGENT, attrName, comp, value);
}
