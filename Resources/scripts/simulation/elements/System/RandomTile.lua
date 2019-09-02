ts = gtsr("RandomTileSet")
valids = GetTilesFromSet(ts)

if #valids == 0 then error("empty TileSet for random tile") end

index = RandomRangeInt(1, #valids + 1)
str("RandomTile", valids[index])
