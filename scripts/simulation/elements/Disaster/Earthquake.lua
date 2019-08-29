slantUp = RandomRangeInt(0,1) == 0
yDivide = gir("GroundLine") + (gir("GroundHeight") / 2)

legalStart = 0
legalEnd = 0

if slantUp then
  legalStart = yDivide
  legalEnd = gir("TileHeight") - 1
else
  legalStart = gir("GroundLine") + 3
  legalEnd = yDivide
end
legalSet = TileSet()
for i=legalStart,legalEnd do
  legalSet = AddTileToSet(legalSet, GetTileAtPosition(0, i))
end

stsr("RandomTileSet", legalSet)
push("System.RandomTile")
startTile = gtr("RandomTile")
DestroyTileSet(legalSet)

if slantUp then
  legalStart = gir("GroundLine") + 3
  legalEnd = yDivide
else
  legalStart = yDivide
  legalEnd = gir("TileHeight") - 1
end
legalSet = TileSet()
for i=legalStart,legalEnd do
  legalSet = AddTileToSet(legalSet, GetTileAtPosition(gir("TileWidth") - 1, i))
end

stsr("RandomTileSet", legalSet)
push("System.RandomTile")
endTile = gtr("RandomTile")
DestroyTileSet(legalSet)

horizontalLine = getStraightLinePath(startTile, endTile)
for _, t in pairs(horizontalLine) do
  t:SetAttributeInt("open", 1)
end



slantRight = RandomRangeInt(0,1) == 0
xDivide = (gir("TileWidth") - 1) / 2

if slantRight then
  legalStart = 0
  legalEnd = xDivide
else
  legalStart = xDivide
  legalEnd = gir("TileWidth") - 1
end
legalSet = TileSet()
for i=legalStart,legalEnd do
  legalSet = AddTileToSet(legalSet, GetTileAtPosition(i, gir("GroundLine")))
end

stsr("RandomTileSet", legalSet)
push("System.RandomTile")
startTile = gtr("RandomTile")
DestroyTileSet(legalSet)

if slantRight then
  legalStart = xDivide
  legalEnd = gir("TileWidth") - 1
else
  legalStart = 0
  legalEnd = xDivide
end
legalSet = TileSet()
for i=legalStart,legalEnd do
  legalSet = AddTileToSet(legalSet, GetTileAtPosition(i, gir("TileHeight") - 1))
end

stsr("RandomTileSet", legalSet)
push("System.RandomTile")
endTile = gtr("RandomTile")
DestroyTileSet(legalSet)

verticalLine = getStraightLinePath(startTile, endTile)
for _, t in pairs(verticalLine) do
  t:SetAttributeInt("open", 1)
end
