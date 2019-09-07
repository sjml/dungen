#include "stdafx.h"
#include "pathfind.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#endif // __clang__
#include <pqueue.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__


typedef struct {
    pqueue_pri_t priority;
    TileData* tile;
    size_t pos;
} TileNode;

static int cmp_pri(pqueue_pri_t next, pqueue_pri_t curr) {
    return (next < curr);
}

static pqueue_pri_t get_pri(void *a) {
    return ((TileNode *) a)->priority;
}

static void set_pri(void *a, pqueue_pri_t pri) {
    ((TileNode *) a)->priority = pri;
}

static size_t get_pos(void *a) {
    return ((TileNode *) a)->pos;
}

static void set_pos(void *a, size_t pos) {
    ((TileNode *) a)->pos = pos;
}

TileData** FindPathThroughValids(TileData* start, TileData* end, TileSet* validTiles) {
    pqueue_t *frontier = pqueue_init(5, cmp_pri, get_pri, set_pri, get_pos, set_pos);
    struct { TileData* key; unsigned long value; } *costs = NULL;
    struct { TileData* key; TileData* value; } *parents = NULL;

    TileNode* startNode = malloc(sizeof(TileNode));
    startNode->priority = INT_MAX;
    startNode->tile = start;
    pqueue_insert(frontier, startNode);
    hmput(costs, start, 0);
    hmput(parents, start, NULL);

    while (!(pqueue_size(frontier) == 0)) {
        TileNode* current = pqueue_pop(frontier);
        if (current->tile == end) {
            free(current);
            break;
        }

        TileData** neighbors = GetTileNeighbors(current->tile);
        for (int n=0; n < arrlen(neighbors); n++) {
            if (!IsTileInSet(validTiles, neighbors[n])) {
                continue;
            }
            unsigned long cost = hmget(costs, current->tile);
            cost += 1; // TODO: have this be smarter about calculating from current to neighbors[n]
            if (hmgeti(costs, neighbors[n]) < 0 || cost < hmget(costs, neighbors[n])) {
                hmput(costs, neighbors[n], cost);
                TileNode* nNode = malloc(sizeof(TileNode));
                nNode->priority = INT_MAX - (cost + GetTileDistance(neighbors[n], end));
                nNode->tile = neighbors[n];
                pqueue_insert(frontier, nNode);
                hmput(parents, neighbors[n], current->tile);
            }
        }
        free(current);
        arrfree(neighbors);
    }

    TileData** pathRev = NULL;
    TileData* next = hmget(parents, end);
    if (next == NULL) {
        return NULL;
    }
    arrpush(pathRev, end);
    while (next != NULL) {
        arrpush(pathRev, next);
        next = hmget(parents, next);
    }
    TileData** path = NULL;
    for (long i = arrlen(pathRev) - 1; i >= 0; i--) {
        arrpush(path, pathRev[i]);
    }
    arrfree(pathRev);

    while (!(pqueue_size(frontier) == 0)) {
        TileNode* c = pqueue_pop(frontier);
        free(c);
    }
    pqueue_free(frontier);
    hmfree(costs);
    hmfree(parents);
    return path;
}

TileData** FindSimplePath(TileData* start, TileData* end) {
    TileSet* ts = GetAllTilesAsSet();
    TileData** ret = FindPathThroughValids(start, end, ts);
    DestroyTileSet(ts);
    return ret;
}

TileData** FindPathThroughAttribute(TileData* start, TileData* end, const char* attrName, AttrComparison comp, const char* value) {
    TileData** vts = GetTilesByAttribute(attrName, comp, value);
    TileSet* ts = NULL;
    for (long i=0; i < arrlen(vts); i++) {
        ts = AddTileToSet(ts, vts[i]);
    }
    arrfree(vts);
    TileData** ret = FindPathThroughValids(start, end, ts);
    DestroyTileSet(ts);
    return ret;
}
