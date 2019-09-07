%module dungen
%{
    #include "../constraints/pathfind.h"
%}

TileData** FindSimplePath(TileData* start, TileData* end);
TileData** FindPathThroughAttribute(TileData* start, TileData* end, const char* attrName, AttrComparison comp, const char* value);
