legalSet = TileSet()
for i=3, gir("TileWidth") - 4 do
  legalSet = AddTileToSet(legalSet, GetTileAtPosition(i, gir("TileHeight") - 1))
end
stsr("RandomTileSet", legalSet)
push("System.RandomTile")

baseTile = gtr("RandomTile")
DestroyTileSet(legalSet)

pocket = makeChamber(baseTile, 3)
pocket:AddTag("magma")


chasmTrace = {}
count = 0

while (count < gir("GroundHeight")) do
  if (count % 2 == 0) then
    table.insert(chasmTrace, NORTHEAST)
  else
    table.insert(chasmTrace, NORTHWEST)
  end
  count = count + 1
end

chasmPath = baseTile:BuildPath(chasmTrace)
for i, tile in ipairs(chasmPath) do
  tile:SetAttributeInt("open", 1)
  if i < (#chasmPath / 3.5) then
    tile:AddTag("magma")
  end
end

topIdx = #chasmPath
topTile = chasmPath[topIdx]
while topTile:HasTags("sky") do
  topIdx = topIdx - 1
  topTile = chasmPath[topIdx]
end

 conePath = { -- build a triangle at the top of the chasm
  NORTHWEST,
  WEST,
  NORTHEAST,
  EAST,
  SOUTHEAST,
  EAST,
  NORTHWEST,
  NORTHWEST,
  WEST,
}

cone = topTile:BuildPath(conePath)
for _, coneTile in pairs(cone) do
  coneTile:AddTag("ground")
  coneTile:RemoveTag("sky")
  coneTile:SetAttributeInt("open", 0)
end
