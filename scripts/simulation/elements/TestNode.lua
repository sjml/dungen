

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
  -- t.color = {0, 1, 0}
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
lazyBasicAssert(4, #tags, "Tag count")
lazyBasicAssert("ground", tags[1], "Tag retrieval 1")
lazyBasicAssert("A", tags[2], "Tag retrieval 2")
lazyBasicAssert("B", tags[3], "Tag retrieval 3")
lazyBasicAssert("C", tags[4], "Tag retrieval 4")

tiles = GetTilesTagged("B, C")
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

ts2 = TileSet()
AddTileToSet(ts2, GetTileAtIndex(0))
AddTileToSet(ts2, GetTileAtIndex(1))
AddTileToSet(ts2, GetTileAtPosition(1, 1))
AddTileToSet(ts2, GetTileAtPosition(3, 2))
ts2:AddTag("Q")
lazyBasicAssert(ts2:HasTags("Q"), true, "TileSet tag check")
lazyBasicAssert(GetTileAtIndex(0):HasTags("Q"), false, "TileSet tags not propagated")
lazyBasicAssert(#GetTileAtIndex(0).memberSets, 1, "Tile membership count")
lazyBasicAssert(GetTileAtIndex(0).memberSets[1].i, ts2.i, "Tile membership check")

SetIntRegister("A", 5682)
SetFloatRegister("B", 3.14)
SetStringRegister("C", "Hello!")
SetTileRegister("D", td)
SetTileSetRegister("E", ts)
lazyBasicAssert(GetIntRegister("A"), 5682, "Integer register check")
lazyBasicAssert(string.format("%.2f", GetFloatRegister("B")), string.format("%.2f", 3.14), "Float register check")
lazyBasicAssert(GetStringRegister("C"), "Hello!", "String register check")
lazyBasicAssert(GetTileRegister("D"), td, "Tile register check")
lazyBasicAssert(GetTileSetRegister("E"), ts, "TileSet register check")
lazyBasicAssert(GetIntRegister("F"), 0, "Null integer register check")
lazyBasicAssert(string.format("%.2f", GetFloatRegister("F")), string.format("%.2f", 0.0), "Null integer register check")
lazyBasicAssert(GetStringRegister("F"), "", "Null string register check")
lazyBasicAssert(GetTileRegister("F"), nil, "Null Tile register check")
lazyBasicAssert(GetTileSetRegister("F"), nil, "Null TileSet set register check")

tdStart = GetTileAtIndex(0)
tdEnd = GetTileAtIndex(1024)
path = FindSimplePath(tdStart, tdEnd)
for _, t in pairs(path) do
  t.color = {0.0, 0.0, 1.0}
end
tdStart.color = {0.0, 1.0, 0.0}
tdEnd.color = {1.0, 0.0, 0.0}

hc = HasAllTags("B, ground")
hc_tagged = hc:GetTiles()
lazyBasicAssert(hc_tagged.isSet, true, "Constraints returning sets")
lazyBasicAssert(#hc_tagged:toList(), 2, "Has tags constraint in isolation 1")
hc2 = HasAllTags("A")
hc_tagged2 = hc2:GetTiles()
lazyBasicAssert(#hc_tagged2:toList(), 1, "Has tags constraint in isolation 2")
hc_tag_combine = hc_tagged:intersection(hc_tagged2)
lazyBasicAssert(#hc_tag_combine:toList(), 1, "Tag constraint combinations")
lazyBasicAssert(hc_tag_combine:toList()[1].i, td.i, "Constraint outcome")
hc3 = HasNoneOfTags("Non-Existent, A")
lazyBasicAssert(true, hc3:CheckTile(td3), "Has none of tags constraint positive")
hc4 = HasNoneOfTags("A, B, C")
lazyBasicAssert(false, hc4:CheckTile(td3), "Has none of tags constraint negative")

ac = HasAttributes("open", GreaterThan, 0)
openTiles = ac:GetTiles()
openTileList = openTiles:toList()
lazyBasicAssert(183, #openTileList, "Attribute queries")
hc = HasAllTags("sky")
hc_tagged = hc:GetTiles()
lazyBasicAssert(#hc_tagged:toList(), #openTileList, "Attributes matching lengths")

td = nil
td2 = nil
td3 = nil
tdStart = nil
tdEnd = nil
path = nil
tiles = nil
tags = nil
ts = nil
ts2 = nil
mems = nil
hc = nil
hc_tagged = nil
hc2 = nil
hc_tagged2 = nil
hc_tag_combine = nil
hc3 = nil
hc4 = nil
ac = nil
collectgarbage() -- just to make sure this doesn't trigger segfaults
