%module dungen
%{
    #include "../constraints/pathfind.h"
%}

TileData** FindSimplePath(TileData* start, TileData* end);
