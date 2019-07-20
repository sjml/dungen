
function lazyBasicAssert(item1, item2, desc)
  if (desc == nil) then
    desc = ""
  else
    desc = "("..desc..")"
  end
  if (item1 == item2) then
    print("✅ OK " .. desc)
    return
  elseif (type(item1) ~= "table" and type(item2) == "table") then
    for _, v in pairs(item2) do
      if (item1 == v) then
        print("✅ OK " .. desc)
        return
      end
    end
  end
  io.stderr:write("❌ FAIL! " .. desc .. "\n")
  io.stderr:write("\t" .. tostring(item1) .. " not equal to " .. tostring(item2) .. "\n")
end

td = GetTileAtPosition(25, 25)
td2 = GetTileAtPosition(25, 27)
td3 = GetTileAtPosition(25, 29)

-- ts = TileSet()
-- lazyBasicAssert(0, GetTileCount(ts), "TileSet count 1")
-- AddTileToSet(ts, td)
-- lazyBasicAssert(1, GetTileCount(ts), "TileSet count 2")
-- AddTileToSet(ts, td2)
-- lazyBasicAssert(2, GetTileCount(ts), "TileSet count 3")

-- lazyBasicAssert(true, IsTileInSet(ts, td), "TileSet membership 1")
-- lazyBasicAssert(true, IsTileInSet(ts, td2), "TileSet membership 2")
-- lazyBasicAssert(false, IsTileInSet(ts, td3), "TileSet membership 3")

-- tiles = GetTiles(ts)
-- lazyBasicAssert(2, #tiles, "TileSet count")
-- for _, t in ipairs(tiles) do
--   t.color = {0, 1, 0}
-- end

-- RemoveTileFromSet(ts, td)
-- lazyBasicAssert(1, GetTileCount(ts), "TileSet removal 1")
-- lazyBasicAssert(false, IsTileInSet(ts, td), "TileSet removal 2")
-- lazyBasicAssert(true, IsTileInSet(ts, td2), "TileSet removal 3")
-- RemoveTileFromSet(ts, td2)
-- lazyBasicAssert(0, GetTileCount(ts), "TileSet removal 4")
-- lazyBasicAssert(false, IsTileInSet(ts, td2), "TileSet removal 5")

-- td:SetAttributeInt("intVal", 5682)
-- td:SetAttributeFloat("floatVal", 3.14)
-- td:SetAttributeString("strVal", "Hello!")
-- lazyBasicAssert(5682, td:GetAttributeInt("intVal"), "Integer attribute")
-- lazyBasicAssert("3.14", string.format("%.2f", td:GetAttributeFloat("floatVal")), "Float attribute")
-- lazyBasicAssert("Hello!", td:GetAttributeString("strVal"), "String attribute")

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

-- tags = td:GetTags()
-- lazyBasicAssert(4, #tags, "Tag count")
-- lazyBasicAssert("ground", tags[1], "Tag retrieval 1")
-- lazyBasicAssert("A", tags[2], "Tag retrieval 2")
-- lazyBasicAssert("B", tags[3], "Tag retrieval 3")
-- lazyBasicAssert("C", tags[4], "Tag retrieval 4")

tiles = GetTilesTagged("B,C")
lazyBasicAssert(2, #tiles, "Multi-tag count")
tileIndices = {tiles[1].i, tiles[2].i}
lazyBasicAssert(td.i,  tileIndices, "Tile retrieval by tag 1")
lazyBasicAssert(td2.i, tileIndices, "Tile retrieval by tag 2")

tiles = GetTilesTagged("C")
lazyBasicAssert(3, #tiles, "Tag count 2")
tileIndices = {tiles[1].i, tiles[2].i, tiles[3].i}
lazyBasicAssert(td.i,  tileIndices, "Tile retrieval by tag 3")
lazyBasicAssert(td2.i, tileIndices, "Tile retrieval by tag 4")
lazyBasicAssert(td3.i, tileIndices, "Tile retrieval by tag 5")

-- ts2 = TileSet()
-- AddTileToSet(ts2, GetTileAtIndex(0))
-- AddTileToSet(ts2, GetTileAtIndex(1))
-- AddTileToSet(ts2, GetTileAtPosition(1, 1))
-- AddTileToSet(ts2, GetTileAtPosition(3, 2))
-- ts2:AddTag("Q")
-- lazyBasicAssert(ts2:HasTags("Q"), true, "TileSet tag check")
-- lazyBasicAssert(GetTileAtIndex(0):HasTags("Q"), false, "TileSet tags not propagated")
-- mems = GetTileAtIndex(0).memberSets
-- lazyBasicAssert(#mems, 1, "Tile membership count")
-- lazyBasicAssert(mems[1].i, ts2.i, "Tile membership check")

-- SetIntRegister("A", 5682)
-- SetFloatRegister("B", 3.14)
-- SetStringRegister("C", "Hello!")
-- SetTileRegister("D", td)
-- SetTileSetRegister("E", ts)
-- lazyBasicAssert(GetIntRegister("A"), 5682, "Integer register check")
-- lazyBasicAssert(string.format("%.2f", GetFloatRegister("B")), string.format("%.2f", 3.14), "Float register check")
-- lazyBasicAssert(GetStringRegister("C"), "Hello!", "String register check")
-- lazyBasicAssert(GetTileRegister("D"), td, "Tile register check")
-- lazyBasicAssert(GetTileSetRegister("E"), ts, "TileSet register check")
-- lazyBasicAssert(GetIntRegister("F"), 0, "Null integer register check")
-- lazyBasicAssert(string.format("%.2f", GetFloatRegister("F")), string.format("%.2f", 0.0), "Null integer register check")
-- lazyBasicAssert(GetStringRegister("F"), "", "Null string register check")
-- lazyBasicAssert(GetTileRegister("F"), nil, "Null Tile register check")
-- lazyBasicAssert(GetTileSetRegister("F"), nil, "Null TileSet set register check")

-- td = nil
-- td2 = nil
-- td3 = nil
-- tiles = nil
-- tags = nil
-- ts = nil
-- ts2 = nil
-- mems = nil
-- collectgarbage() -- just to make sure this doesn't trigger segfaults
