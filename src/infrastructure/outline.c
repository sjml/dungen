#include "stdafx.h"
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


Outline* CreateOutline(TileSet* ts, float thickness, int type) {
    Outline* o = malloc(sizeof(Outline));
    o->color.r = 1.0f;
    o->color.g = 0.0f;
    o->color.b = 0.0f;
    o->color.a = 1.0f;
    o->numPoints = 0;
    o->buffer = (sg_buffer){0};
    o->bindings = (sg_bindings){0};

    gbVec2** PL = GetWorldPointList();

    struct { Vec2i key; int value; } *outlineEdges = NULL;

    for (int t = 0; t < hmlen(ts); t++) {
        TileData* tile = ts[t].key;
        Vec2i edgeIndices[6];

        // nw
        edgeIndices[0].x = 2 * tile->meta->hexPos.x;
        edgeIndices[0].y = 2 * tile->meta->hexPos.y;
        if (tile->meta->hexPos.y % 2 == 1) {
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
        edgeIndices[4].x = tile->meta->hexPos.x;
        edgeIndices[4].y = (2 * tile->meta->hexPos.y) + 1;
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

    float** pointLists = NULL;
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
        float halfThickness = GetWorldScale() * thickness;

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
                    if (type == 0) {
                        // center
                        arrpush(points, cv->x + normal.x);
                        arrpush(points, cv->y + normal.y);
                        arrpush(points, cv->x - normal.x);
                        arrpush(points, cv->y - normal.y);
                    }
                    else if (type == 1) {
                        // inner
                        arrpush(points, cv->x);
                        arrpush(points, cv->y);
                        arrpush(points, cv->x - (normal.x * 2.0f));
                        arrpush(points, cv->y - (normal.y * 2.0f));
                    }
                    else if (type == -1) {
                        // outer
                        arrpush(points, cv->x + (normal.x * 2.0f));
                        arrpush(points, cv->y + (normal.y * 2.0f));
                        arrpush(points, cv->x);
                        arrpush(points, cv->y);
                    }

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
        if (type == 0) {
            // center
            arrpush(points, cv->x + normal.x);
            arrpush(points, cv->y + normal.y);
            arrpush(points, cv->x - normal.x);
            arrpush(points, cv->y - normal.y);

            points[0] = cv->x + normal.x;
            points[1] = cv->y + normal.y;
            points[2] = cv->x - normal.x;
            points[3] = cv->y - normal.y;
        }
        else if (type == 1) {
            // inner
            arrpush(points, cv->x);
            arrpush(points, cv->y);
            arrpush(points, cv->x - (normal.x * 2.0f));
            arrpush(points, cv->y - (normal.y * 2.0f));

            points[0] = cv->x;
            points[1] = cv->y;
            points[2] = cv->x - (normal.x * 2.0f);
            points[3] = cv->y - (normal.y * 2.0f);
        }
        else if (type == -1) {
            // outer
            arrpush(points, cv->x + (normal.x * 2.0f));
            arrpush(points, cv->y + (normal.y * 2.0f));
            arrpush(points, cv->x);
            arrpush(points, cv->y);

            points[0] = cv->x + (normal.x * 2.0f);
            points[1] = cv->y + (normal.y * 2.0f);
            points[2] = cv->x;
            points[3] = cv->y;
        }

        arrpush(pointLists, points);
    }

    // there are probably better ways to fill this, but this is stapled
    //   on to the end of some logic made when this was all using the
    //   fixed-function pipeline.
    float* bufferList = NULL;
    for (long i=0; i < arrlen(pointLists); i++) {
        long pi = 0;
        for (; pi < arrlen(pointLists[i]); pi++) {
            arrpush(bufferList, pointLists[i][pi]);
        }

        if (i < arrlen(pointLists) - 1 && arrlen(pointLists[i+1]) > 0) {
            // push degenerate triangle to connect to separated region
            arrpush(bufferList, pointLists[i][pi-2]);
            arrpush(bufferList, pointLists[i][pi-1]);
            arrpush(bufferList, pointLists[i+1][0]);
            arrpush(bufferList, pointLists[i+1][1]);
        }
    }
    if (arrlen(bufferList) > 0) {
        o->numPoints = (size_t)arrlen(bufferList) / 2;
        o->buffer = sg_make_buffer(&(sg_buffer_desc){
            .data = {
                .ptr = bufferList,
                .size = sizeof(float)*(int)arrlen(bufferList)
            }
        });
        o->bindings = (sg_bindings) {
            .vertex_buffers[0] = o->buffer
        };
    }
    arrfree(bufferList);

    for (int i = 0; i < arrlen(pointLists); i++) {
        arrfree(pointLists[i]);
    }
    arrfree(pointLists);

    return o;
}

void DestroyOutline(Outline* o) {
    sg_destroy_buffer(o->buffer);
    free(o);
}
