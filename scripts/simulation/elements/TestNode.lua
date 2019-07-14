
td = dungen.GetTileAtPosition(25, 25)
td2 = dungen.GetTileAtPosition(25, 27)
td3 = dungen.GetTileAtPosition(25, 29)

ts = dungen.TileSet()
print(string.format("0:\t%d", dungen.GetTileCount(ts)))
dungen.AddTileToSet(ts, td)
print(string.format("1:\t%d", dungen.GetTileCount(ts)))
dungen.AddTileToSet(ts, td2)
print(string.format("2:\t%d", dungen.GetTileCount(ts)))

print("true: ", dungen.IsTileInSet(ts, td))
print("true: ", dungen.IsTileInSet(ts, td2))
print("false: ", dungen.IsTileInSet(ts, td3))

tiles = dungen.GetTiles(ts)
print("2:\t", #tiles)
for _, t in ipairs(tiles) do
  t.color = {0, 1, 0}
end

dungen.RemoveTileFromSet(ts, td)
print(string.format("1:\t%d", dungen.GetTileCount(ts)))
print("false: ", dungen.IsTileInSet(ts, td))
print("true: ", dungen.IsTileInSet(ts, td2))
dungen.RemoveTileFromSet(ts, td2)
print(string.format("0:\t%d", dungen.GetTileCount(ts)))
print("false: ", dungen.IsTileInSet(ts, td2))

td:SetAttributeInt("intVal", 5682)
td:SetAttributeFloat("floatVal", 3.14)
td:SetAttributeString("strVal", "Hello!")
print(string.format("5682:\t%d", td:GetAttributeInt("intVal")))
print(string.format("3.14:\t%.2f", td:GetAttributeFloat("floatVal")))
print("Hello!:", td:GetAttributeString("strVal"))

-- resulting tags:
-- td  A B C
-- td2 B C
-- td3 C
td:AddTag("A")
td:AddTag("B")
td:AddTag("B")
td2:AddTag("B")
td2:AddTag("C")
td3:AddTag("C")
td:AddTag("C")
td:AddTag("D")
td:RemoveTag("D")
td:RemoveTag("D")

tags = td:GetTags()
print("3: ", #tags)
print("A:", tags[1])
print("B:", tags[2])
print("C:", tags[3])

tiles = dungen.GetTilesTagged("B,C")
print("2:", #tiles)
print(string.format("%d:\t%d", td.i, tiles[1].i))
print(string.format("%d:\t%d", td2.i, tiles[2].i))

tiles = dungen.GetTilesTagged("C")
print("3:", #tiles)
print(string.format("%d:\t%d", td.i, tiles[1].i))
print(string.format("%d:\t%d", td2.i, tiles[2].i))
print(string.format("%d:\t%d", td3.i, tiles[3].i))

collectgarbage() -- just to make sure this doesn't trigger segfaults
