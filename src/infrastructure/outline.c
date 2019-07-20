#include "../stdafx.h"
#include "outline.h"

#include "rendering.h"

typedef struct {
    Vec2i EdgePos;
    Vec2i StartIdx;
    Vec2i EndIdx;
} Edge;

int CompareEdges(Edge *e1, Edge *e2) {
    if (e1->EdgePos.x == e2->EdgePos.x && e1->EdgePos.y == e2->EdgePos.y) {
        return 0;
    }
    if (e1->EdgePos.y < e2->EdgePos.y)
    {
        return -1;
    }
    if (e1->EdgePos.y == e2->EdgePos.y && e1->EdgePos.x < e2->EdgePos.x)
    {
        return -1;
    }
    return 1;
}

Edge CreateEdge(Vec2i ePos) {
    Edge ret;
    ret.EdgePos = ePos;
    if (ePos.y % 2 == 0) {
        ret.StartIdx.y = ePos.y / 2;
        ret.StartIdx.x = ePos.x;
        ret.EndIdx.y = ePos.y / 2;
        ret.EndIdx.x = ePos.x + 1;
    }
    else {
        ret.StartIdx.y = (ePos.y - 1) / 2;
        ret.StartIdx.x = ePos.x * 2;
        ret.EndIdx.y = ((ePos.y - 1) / 2) + 1;
        ret.EndIdx.x = ePos.x * 2;

        if ((ePos.y + 1) % 4 == 0) {
            ret.StartIdx.x += 1;
            ret.EndIdx.x += 1;
        }
    }

    return ret;
}

bool DoEdgesShareVertex(Edge *e1, Edge *e2, Vec2i *shared, gbVec2 *crossVec) {
    gbVec2** pl = GetWorldPointList();

    // if e1 start == e2 end
    if (   e1->StartIdx.x == e2->EndIdx.x
        && e1->StartIdx.y == e2->EndIdx.y) {
        shared->x = e1->StartIdx.x;
        shared->y = e1->StartIdx.y;
        gbVec2* start = &pl[e2->StartIdx.y][e2->StartIdx.x];
        gbVec2* end = &pl[e1->EndIdx.y][e1->EndIdx.x];
        crossVec->x = start->x - end->x;
        crossVec->y = start->y - end->y;
        return true;
    }
    // if e1 end == e2 start
    if (   e1->EndIdx.x == e2->StartIdx.x
        && e1->EndIdx.y == e2->StartIdx.y) {
        shared->x = e1->EndIdx.x;
        shared->y = e1->EndIdx.y;
        gbVec2* start = &pl[e2->EndIdx.y][e2->EndIdx.x];
        gbVec2* end = &pl[e1->StartIdx.y][e1->StartIdx.x];
        crossVec->x = start->x - end->x;
        crossVec->y = start->y - end->y;
        return true;
    }
    // if e1 start == e2 start
    if (   e1->StartIdx.x == e2->StartIdx.x
        && e1->StartIdx.y == e2->StartIdx.y) {
        shared->x = e1->StartIdx.x;
        shared->y = e1->StartIdx.y;
        gbVec2* start = &pl[e2->EndIdx.y][e2->EndIdx.x];
        gbVec2* end = &pl[e1->EndIdx.y][e1->EndIdx.x];
        crossVec->x = start->x - end->x;
        crossVec->y = start->y - end->y;
        return true;
    }
    // if e1 end == e2 end
    if (   e1->EndIdx.x == e2->EndIdx.x
        && e1->EndIdx.y == e2->EndIdx.y) {
        shared->x = e1->EndIdx.x;
        shared->y = e1->EndIdx.y;
        gbVec2* start = &pl[e2->StartIdx.y][e2->StartIdx.x];
        gbVec2* end = &pl[e1->StartIdx.y][e1->StartIdx.x];
        crossVec->x = start->x - end->x;
        crossVec->y = start->y - end->y;
        return true;
    }
    return false;
}


Outline* CreateOutline(TileSet* ts) {
    Outline* o = malloc(sizeof(Outline));
    o->color.r = 1.0f;
    o->color.g = 0.0f;
    o->color.b = 0.0f;
    o->pointLists = NULL;

    TileData** tiles = GetTiles(ts);
    gbVec2** PL = GetWorldPointList();

    struct { Vec2i key; int value; } *outlineEdges = NULL;

    for (int t = 0; t < arrlen(tiles); t++) {
        TileData* tile = tiles[t];
        Vec2i edgeIndices[6];

        // nw
        edgeIndices[0].x = 2 * tile->hexPos.x;
        edgeIndices[0].y = 2 * tile->hexPos.y;
        if (tile->hexPos.y % 2 == 1) {
            edgeIndices[0].x += 1;
        }
        // ne
        edgeIndices[1].x = edgeIndices[0].x + 1;
        edgeIndices[1].y = edgeIndices[0].y;
        // se
        edgeIndices[2].x = edgeIndices[1].x;
        edgeIndices[2].y = edgeIndices[1].y + 2;
        // sw
        edgeIndices[3].x = edgeIndices[2].x - 1;
        edgeIndices[3].y = edgeIndices[2].y;
        // w
        edgeIndices[4].x = tile->hexPos.x;
        edgeIndices[4].y = (2 * tile->hexPos.y) + 1;
        // e
        edgeIndices[5].x = edgeIndices[4].x + 1;
        edgeIndices[5].y = edgeIndices[4].y;

        for (int ei = 0; ei < 6; ei++) {
            if (hmgeti(outlineEdges, edgeIndices[ei]) == -1) {
                hmput(outlineEdges, edgeIndices[ei], 1);
            }
            else {
                int count = hmget(outlineEdges, edgeIndices[ei]);
                hmput(outlineEdges, edgeIndices[ei], count + 1);
            }
        }
    }

    for (int i = (int)hmlen(outlineEdges) - 1; i >= 0 ; i--) {
        if (outlineEdges[i].value > 1) {
            hmdel(outlineEdges, outlineEdges[i].key);
        }
    }

    Edge* edges = NULL;
    for (int i = 0; i < hmlen(outlineEdges); i++) {
        arrpush(edges, CreateEdge(outlineEdges[i].key));
    }
    hmfree(outlineEdges);

    while (arrlen(edges) > 0) {
        float* points = NULL;
        Edge startingEdge = arrpop(edges);
        arrpush(points, PL[startingEdge.StartIdx.y][startingEdge.StartIdx.x].x);
        arrpush(points, PL[startingEdge.StartIdx.y][startingEdge.StartIdx.x].y);
        arrpush(points, PL[startingEdge.StartIdx.y][startingEdge.StartIdx.x].x);
        arrpush(points, PL[startingEdge.StartIdx.y][startingEdge.StartIdx.x].y);

        Edge edgeCheck = startingEdge;
        Edge first = startingEdge;
        Edge lastMatched = startingEdge;
        Vec2i checkVert;
        gbVec2 crossVector;
        float halfThickness = GetWorldScale() * 0.15f;

        while (arrlen(edges) > 0) {
            int currentSize = (int)arrlen(edges);
            for (int i = (int)arrlen(edges) - 1; i >= 0; i--) {
                Edge e = edges[i];
                if (DoEdgesShareVertex(&edgeCheck, &e, &checkVert, &crossVector)) {
                    lastMatched = e;
                    gbVec2 normal = { -crossVector.y, crossVector.x };
                    gb_vec2_norm(&normal, normal);
                    gb_vec2_mul(&normal, normal, halfThickness);

                    gbVec2* cv = &PL[checkVert.y][checkVert.x];
                    arrpush(points, cv->x + normal.x);
                    arrpush(points, cv->y + normal.y);
                    arrpush(points, cv->x - normal.x);
                    arrpush(points, cv->y - normal.y);

                    edgeCheck = e;
                    arrdel(edges, i);
                    break;
                }
            }
            if (currentSize == arrlen(edges)) {
                // end of a loop
                break;
            }
        }

        DoEdgesShareVertex(&lastMatched, &first, &checkVert, &crossVector);
        gbVec2 normal = { -crossVector.y, crossVector.x };
        gb_vec2_norm(&normal, normal);
        gb_vec2_mul(&normal, normal, halfThickness);

        gbVec2* cv = &PL[checkVert.y][checkVert.x];
        arrpush(points, cv->x + normal.x);
        arrpush(points, cv->y + normal.y);
        arrpush(points, cv->x - normal.x);
        arrpush(points, cv->y - normal.y);

        points[0] = cv->x + normal.x;
        points[1] = cv->y + normal.y;
        points[2] = cv->x - normal.x;
        points[3] = cv->y - normal.y;

        arrpush(o->pointLists, points);
    }

    arrfree(tiles);

    return o;
}

void DestroyOutline(Outline* o) {
    for (int i = 0; i < arrlen(o->pointLists); i++) {
        arrfree(o->pointLists[i]);
    }
    arrfree(o->pointLists);
    free(o);
}

void RenderOutline(Outline* o) {
    if (arrlen(o->pointLists) == 0) {
        return;
    }

    glColor3fv(o->color.e);

    glEnableClientState(GL_VERTEX_ARRAY);

    glLineWidth(5.0f);

    for (int i = 0; i < arrlen(o->pointLists); i++) {
        if (arrlen(o->pointLists[0]) == 0) {
            continue;
        }

        glVertexPointer(2, GL_FLOAT, 2 * sizeof(float), o->pointLists[i]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (int)arrlen(o->pointLists[i]) / 2);
    }
}
