
td = dungen.GetTileAtPosition(25, 25)
td2 = dungen.GetTileAtPosition(25, 27)
td3 = dungen.GetTileAtPosition(25, 29)

ts = dungen.TileSet()
print("0.0:", dungen.GetTileCount(ts))
dungen.AddTileToSet(ts, td)
print("1.0: ", dungen.GetTileCount(ts))
dungen.AddTileToSet(ts, td2)
print("2.0: ", dungen.GetTileCount(ts))

print("true: ", dungen.IsTileInSet(ts, td))
print("true: ", dungen.IsTileInSet(ts, td2))
print("false: ", dungen.IsTileInSet(ts, td3))

tiles = dungen.GetTiles(ts)
print("2:\t", #tiles)
for _, t in ipairs(tiles) do
  t.color = {0, 1, 0}
end

dungen.RemoveTileFromSet(ts, td)
print("1.0: ", dungen.GetTileCount(ts))
print("false: ", dungen.IsTileInSet(ts, td))
print("true: ", dungen.IsTileInSet(ts, td2))
dungen.RemoveTileFromSet(ts, td2)
print("0.0: ", dungen.GetTileCount(ts))
print("false: ", dungen.IsTileInSet(ts, td2))
