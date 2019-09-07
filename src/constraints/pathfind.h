#pragma once

#include "../infrastructure/world.h"
#include "../infrastructure/attributes.h"

TileData** FindSimplePath(TileData* start, TileData* end);
// TileData** FindPathThroughTags(TileData* start, TileData* end, const char* tags);
TileData** FindPathThroughAttribute(TileData* start, TileData* end, const char* attrName, AttrComparison comp, const char* value);
