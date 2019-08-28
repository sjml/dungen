local padding = 5
local legalSet = TileSet()
for i= padding, gir("TileWidth") - (padding + 1) do
  legalSet = AddTileToSet(legalSet, GetTileAtPosition(i, gir("GroundLine")))
end
stsr("RandomTileSet", legalSet)
push("System.RandomTile")

baseTile = gtr("RandomTile")
DestroyTileSet(legalSet)

circle = baseTile:GetCircle(2)

crater = CreateRegion()
crater:AddTag("FallenStar")
for _, tile in pairs(circle) do
  tile:AddTag("sky")
  tile:SetAttributeInt("open", 1)
  crater:AddTile(tile)
end

landingTile = baseTile:GetNeighbor(SOUTHEAST):GetNeighbor(SOUTHWEST)
landingTile:SetAttributeInt("treasure", 1 + landingTile:GetAttributeInt("treasure"))
landingTile:AddTag("meteorite")
landingTile:SetAttributeInt("open", 0)

-- right tunnel
startTile = landingTile:GetNeighbor(EAST):GetNeighbor(EAST)
startTile:SetAttributeInt("open", 1)
tunnelPath = {SOUTHEAST, SOUTHEAST}
tunnelTiles = startTile:BuildPath(tunnelPath)
for _, t in pairs(tunnelTiles) do
  t:SetAttributeInt("open", 1)
end

-- left tunnel
startTile = landingTile:GetNeighbor(WEST):GetNeighbor(WEST)
startTile:SetAttributeInt("open", 1)
tunnelPath = {SOUTHWEST, SOUTHWEST}
tunnelTiles = startTile:BuildPath(tunnelPath)
for _, t in pairs(tunnelTiles) do
  t:SetAttributeInt("open", 1)
end

-- middle tunnel
if RandomRangeInt(0,1) == 0 then
  startTile = landingTile:GetNeighbor(SOUTHWEST)
else
  startTile = landingTile:GetNeighbor(SOUTHEAST)
end
startTile:SetAttributeInt("open", 1)
tunnelPath = {SOUTHWEST, SOUTHEAST}
tunnelTiles = startTile:BuildPath(tunnelPath)
for _, t in pairs(tunnelTiles) do
  t:SetAttributeInt("open", 1)
end

