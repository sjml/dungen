
function lazyBasicAssert(item1, item2, desc)
  if (desc == nil) then
    desc = ""
  else
    desc = "("..desc..")"
  end
  if (item1 == item2) then
    print("✅ OK " .. desc)
  else
    io.stderr:write("❌ FAIL! " .. desc .. "\n")
  end
end

td = GetTileAtPosition(25, 25)
td2 = GetTileAtPosition(25, 27)
td3 = GetTileAtPosition(25, 29)

ts = TileSet()
lazyBasicAssert(0, GetTileCount(ts), "TileSet count 1")
AddTileToSet(ts, td)
lazyBasicAssert(1, GetTileCount(ts), "TileSet count 2")
AddTileToSet(ts, td2)
lazyBasicAssert(2, GetTileCount(ts), "TileSet count 3")

lazyBasicAssert(true, IsTileInSet(ts, td), "TileSet membership 1")
lazyBasicAssert(true, IsTileInSet(ts, td2), "TileSet membership 2")
lazyBasicAssert(false, IsTileInSet(ts, td3), "TileSet membership 3")

tiles = GetTiles(ts)
lazyBasicAssert(2, #tiles, "TileSet count")
for _, t in ipairs(tiles) do
  t.color = {0, 1, 0}
end

RemoveTileFromSet(ts, td)
lazyBasicAssert(1, GetTileCount(ts), "TileSet removal 1")
lazyBasicAssert(false, IsTileInSet(ts, td), "TileSet removal 2")
lazyBasicAssert(true, IsTileInSet(ts, td2), "TileSet removal 3")
RemoveTileFromSet(ts, td2)
lazyBasicAssert(0, GetTileCount(ts), "TileSet removal 4")
lazyBasicAssert(false, IsTileInSet(ts, td2), "TileSet removal 5")

td:SetAttributeInt("intVal", 5682)
td:SetAttributeFloat("floatVal", 3.14)
td:SetAttributeString("strVal", "Hello!")
lazyBasicAssert(5682, td:GetAttributeInt("intVal"), "Integer attribute")
lazyBasicAssert("3.14", string.format("%.2f", td:GetAttributeFloat("floatVal")), "Float attribute")
lazyBasicAssert("Hello!", td:GetAttributeString("strVal"), "String attribute")

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
lazyBasicAssert(3, #tags, "Tag count")
lazyBasicAssert("A", tags[1], "Tag retrieval 1")
lazyBasicAssert("B", tags[2], "Tag retrieval 2")
lazyBasicAssert("C", tags[3], "Tag retrieval 3")

tiles = GetTilesTagged("B,C")
lazyBasicAssert(2, #tiles, "Multi-tag count")
lazyBasicAssert(td.i, tiles[1].i, "Tile retrieval by tag 1")
lazyBasicAssert(td2.i, tiles[2].i, "Tile retrieval by tag 2")

tiles = GetTilesTagged("C")
lazyBasicAssert(3, #tiles, "Tag count 2")
lazyBasicAssert(td.i,  tiles[1].i, "Tile retrieval by tag 3")
lazyBasicAssert(td2.i, tiles[2].i, "Tile retrieval by tag 4")
lazyBasicAssert(td3.i, tiles[3].i, "Tile retrieval by tag 5")

-- no asserts for this, just checking it doesn't trash memory
--   (and display can be checked by commenting last line)
ts = TileSet()
AddTileToSet(ts, GetTileAtIndex(0))
AddTileToSet(ts, GetTileAtIndex(1))
AddTileToSet(ts, GetTileAtPosition(1, 1))
AddTileToSet(ts, GetTileAtPosition(3, 2))
o = CreateOutline(ts)
AddOutline(o)
DestroyOutline(o)

collectgarbage() -- just to make sure this doesn't trigger segfaults
