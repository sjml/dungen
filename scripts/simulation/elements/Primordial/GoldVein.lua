
-- This is somewhat silly, but lets it get intercepted by the picker

legalSet = TileSet()
for i=gir("GroundLine"), gir("TileHeight") - 1 do
  legalSet = AddTileToSet(legalSet, GetTileAtPosition(0, i))
end
stsr("RandomTileSet", legalSet)
push("System.RandomTile")

startTile = gtr("RandomTile")
DestroyTileSet(legalSet)

-- If the starting point was near the surface, ensure the other side dips a bit
offset = 0
if (startTile.hexPos.y - gir("GroundLine") < 3) then
  offset = 5
end

legalSet = TileSet()
for i=gir("GroundLine") + offset, gir("TileHeight") - 1 do
  legalSet = AddTileToSet(legalSet, GetTileAtPosition(gir("TileWidth") - 1, i))
end
stsr("RandomTileSet", legalSet)
push("System.RandomTile")

endTile = gtr("RandomTile")
DestroyTileSet(legalSet)

path = getStraightLinePath(startTile, endTile)

for _, t in pairs(path) do
  t:AddTag("gold")
  t:SetAttributeInt("treasure", 1 + t:GetAttributeInt("treasure"))
end
