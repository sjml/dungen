sir("testingInProgress", 1)

errCount = 0

td = GetTileAtPosition(25, 25)
td2 = GetTileAtPosition(25, 27)
td3 = GetTileAtPosition(25, 29)

ts = TileSet()
errCount = errCount + basicAssert(0, GetTileSetCount(ts), "TileSet count 1")
ts = AddTileToSet(ts, td)
errCount = errCount + basicAssert(1, GetTileSetCount(ts), "TileSet count 2")
ts = AddTileToSet(ts, td2)
errCount = errCount + basicAssert(2, GetTileSetCount(ts), "TileSet count 3")

errCount = errCount + basicAssert(true, IsTileInSet(ts, td), "TileSet membership 1")
errCount = errCount + basicAssert(true, IsTileInSet(ts, td2), "TileSet membership 2")
errCount = errCount + basicAssert(false, IsTileInSet(ts, td3), "TileSet membership 3")

tiles = GetTilesFromSet(ts)
errCount = errCount + basicAssert(2, #tiles, "TileSet count")
for _, t in ipairs(tiles) do
  -- t.color = {0, 1, 0}
end

ts = RemoveTileFromSet(ts, td)
errCount = errCount + basicAssert(1, GetTileSetCount(ts), "TileSet removal 1")
errCount = errCount + basicAssert(false, IsTileInSet(ts, td), "TileSet removal 2")
errCount = errCount + basicAssert(true, IsTileInSet(ts, td2), "TileSet removal 3")
ts = RemoveTileFromSet(ts, td2)
errCount = errCount + basicAssert(0, GetTileSetCount(ts), "TileSet removal 4")
errCount = errCount + basicAssert(false, IsTileInSet(ts, td2), "TileSet removal 5")

td:SetAttributeInt("intVal", 5682)
td:SetAttributeFloat("floatVal", 3.14)
td:SetAttributeString("strVal", "Hello!")
errCount = errCount + basicAssert(5682, td:GetAttributeInt("intVal"), "Integer attribute")
errCount = errCount + basicAssert("3.14", string.format("%.2f", td:GetAttributeFloat("floatVal")), "Float attribute")
errCount = errCount + basicAssert("Hello!", td:GetAttributeString("strVal"), "String attribute")

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
errCount = errCount + basicAssert(4, #tags, "Tag count")
errCount = errCount + basicAssert("ground", tags[1], "Tag retrieval 1")
errCount = errCount + basicAssert("A", tags[2], "Tag retrieval 2")
errCount = errCount + basicAssert("B", tags[3], "Tag retrieval 3")
errCount = errCount + basicAssert("C", tags[4], "Tag retrieval 4")

tiles = GetTilesTagged("B, C")
errCount = errCount + basicAssert(2, #tiles, "Multi-tag count")
tileIndices = {tiles[1].meta.i, tiles[2].meta.i}
errCount = errCount + basicAssert(td.meta.i,  tileIndices, "Tile retrieval by tag 1")
errCount = errCount + basicAssert(td2.meta.i, tileIndices, "Tile retrieval by tag 2")

tiles = GetTilesTagged("C")
errCount = errCount + basicAssert(3, #tiles, "Tag count 2")
tileIndices = {tiles[1].meta.i, tiles[2].meta.i, tiles[3].meta.i}
errCount = errCount + basicAssert(td.meta.i,  tileIndices, "Tile retrieval by tag 3")
errCount = errCount + basicAssert(td2.meta.i, tileIndices, "Tile retrieval by tag 4")
errCount = errCount + basicAssert(td3.meta.i, tileIndices, "Tile retrieval by tag 5")

reg = Region()
AddTileToRegion(reg, GetTileAtIndex(0))
AddTileToRegion(reg, GetTileAtIndex(1))
AddTileToRegion(reg, GetTileAtPosition(1, 1))
AddTileToRegion(reg, GetTileAtPosition(3, 2))
reg:AddTag("Q")
errCount = errCount + basicAssert(reg:HasTags("Q"), true, "Region tag check")
errCount = errCount + basicAssert(GetTileAtIndex(0):HasTags("Q"), false, "Region tags not propagated")
errCount = errCount + basicAssert(#GetTileAtIndex(0).meta.memberRegions, 1, "Tile membership count")
errCount = errCount + basicAssert(GetTileAtIndex(0).meta.memberRegions[1].i, reg.i, "Tile membership check")

aList = GetAllAgents()
errCount = errCount + basicAssert(#aList, 0, "No agents to start off")
agent = Agent()
aList = GetAllAgents()
errCount = errCount + basicAssert(#aList, 1, "Creating agent")
errCount = errCount + basicAssert(#agent:GetTags(), 0, "Initial agent has no tags")
agent:AddTag("secret")
errCount = errCount + basicAssert(#agent:GetTags(), 1, "Tagging agent")
secretAgents = GetAgentsTagged("secret")
errCount = errCount + basicAssert(#secretAgents, 1, "Getting tagged agents")
errCount = errCount + basicAssert(secretAgents[1], agent, "Getting right tagged agent")
agent:SetAttributeInt("codename", 007) -- (will just be 7, but you know what I mean)
errCount = errCount + basicAssert(agent:GetAttributeInt("codename"), 007, "Agent integer attribute")
agent:SetAttributeFloat("floatname", 0.07)
errCount = errCount + basicAssert(agent:GetAttributeFloat("floatname") - 0.07 < 0.00000001, true, "Agent float attribute")
agent:SetAttributeString("realname", "bond")
errCount = errCount + basicAssert(agent:GetAttributeString("realname"), "bond", "Agent string attribute")

SetIntRegister("A", 5682)
SetFloatRegister("B", 3.14)
SetStringRegister("C", "Hello!")
SetTileRegister("D", td)
SetTileSetRegister("E", ts)
errCount = errCount + basicAssert(GetIntRegister("A"), 5682, "Integer register check")
errCount = errCount + basicAssert(string.format("%.2f", GetFloatRegister("B")), string.format("%.2f", 3.14), "Float register check")
errCount = errCount + basicAssert(GetStringRegister("C"), "Hello!", "String register check")
errCount = errCount + basicAssert(GetTileRegister("D"), td, "Tile register check")
errCount = errCount + basicAssert(GetTileSetRegister("E"), ts, "TileSet register check")
errCount = errCount + basicAssert(GetIntRegister("F"), 0, "Null integer register check")
errCount = errCount + basicAssert(string.format("%.2f", GetFloatRegister("F")), string.format("%.2f", 0.0), "Null integer register check")
errCount = errCount + basicAssert(GetStringRegister("F"), "", "Null string register check")
errCount = errCount + basicAssert(GetTileRegister("F"), nil, "Null Tile register check")
errCount = errCount + basicAssert(GetTileSetRegister("F"), nil, "Null TileSet set register check")

tdStart = GetTileAtIndex(0)
tdEnd = GetTileAtIndex(1024)
path = FindSimplePath(tdStart, tdEnd)
for _, t in pairs(path) do
  t.draw.color = {0.0, 0.0, 1.0}
end
tdStart.draw.color = {0.0, 1.0, 0.0}
tdEnd.draw.color = {1.0, 0.0, 0.0}

hc = HasAllTags("B, ground")
hc_tagged = hc:GetPassingTiles()
errCount = errCount + basicAssert(hc_tagged.isSet, true, "Constraints returning sets")
errCount = errCount + basicAssert(#hc_tagged:toList(), 2, "Has tags constraint in isolation 1")
hc2 = HasAllTags("A")
hc_tagged2 = hc2:GetPassingTiles()
errCount = errCount + basicAssert(#hc_tagged2:toList(), 1, "Has tags constraint in isolation 2")
hc_tag_combine = hc_tagged:intersection(hc_tagged2)
errCount = errCount + basicAssert(#hc_tag_combine:toList(), 1, "Tag constraint combinations")
errCount = errCount + basicAssert(hc_tag_combine:toList()[1].meta.i, td.meta.i, "Constraint outcome")
hc3 = HasNoneOfTags("Non-Existent, A")
errCount = errCount + basicAssert(true, hc3:CheckTile(td3.meta.i), "Has none of tags constraint positive")
hc4 = HasNoneOfTags("A, B, C")
errCount = errCount + basicAssert(false, hc4:CheckTile(td3.meta.i), "Has none of tags constraint negative")

ac = HasAttributes("open", GreaterThan, 0)
openTiles = ac:GetPassingTiles()
openTileList = openTiles:toList()
errCount = errCount + basicAssert(305, #openTileList, "Attribute queries")
hc = HasAllTags("sky")
hc_tagged = hc:GetPassingTiles()
errCount = errCount + basicAssert(#hc_tagged:toList(), #openTileList, "Attributes matching lengths")

td = GetTileAtIndex(0)
circle1 = td:GetCircle(0)
errCount = errCount + basicAssert(#circle1, 1, "Circle radius of 0")
circle2 = td:GetCircle(1)
errCount = errCount + basicAssert(#circle2, 3, "Corner circle radius of 1")
circle3 = td:GetCircle(2)
errCount = errCount + basicAssert(#circle3, 7, "Corner circle radius of 2")
circle4 = td2:GetCircle(1)
errCount = errCount + basicAssert(#circle4, 7, "Circle radius of 1")
circle5 = td2:GetCircle(2)
errCount = errCount + basicAssert(#circle5, 19, "Circle radius of 2")


reg2 = Region()

errCount = errCount + basicAssert(reg2:GetParent() == nil, true, "Region parent is initially null")
errCount = errCount + basicAssert(#reg:GetChildren(), 0, "Regions initially have no children")
reg2:SetParent(reg)
errCount = errCount + basicAssert(reg2:GetParent() == nil, false, "Setting region parent")
errCount = errCount + basicAssert(reg2:GetParent(), reg, "Setting *correct* region parent")
errCount = errCount + basicAssert(#reg:GetChildren(), 1, "Getting region children")
errCount = errCount + basicAssert(reg:GetChildren()[1], reg2, "Getting *correct* region children")
DestroyRegion(reg2)
errCount = errCount + basicAssert(#reg:GetChildren(), 0, "Destroying child region removes it from parent")


td = nil
td2 = nil
td3 = nil
tdStart = nil
tdEnd = nil
path = nil
tiles = nil
reg = nil
reg2 = nil
agent = nil
aList = nil
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
circle1 = nil
circle2 = nil
circle3 = nil
circle4 = nil
circle5 = nil
collectgarbage() -- just to make sure this doesn't trigger segfaults

sir("testingInProgress", 0)
reportErrors(errCount)
quit(nil, errCount)
