local cavernCenter1 = GetTileAtPosition(5, 45)
local chamber = makeChamber(cavernCenter1, 1)
chamber:AddTag("natural")

local cavernCenter2 = GetTileAtPosition(20, 23)
local chamber = makeChamber(cavernCenter2, 1)
chamber:AddTag("natural")

local midPoint = GetTileAtPosition(20, 45)

str("TunnelStart", cavernCenter1)
str("TunnelEnd"  , midPoint)
push("System.Tunnel")
str("TunnelStart", midPoint)
str("TunnelEnd"  , cavernCenter2)
push("System.Tunnel")

local wrongPath = FindSimplePath(cavernCenter1, cavernCenter2)
for _, tile in pairs(wrongPath) do
  tile.color = {1, 0, 0}
end

local rightPath = FindPathThroughAttribute(cavernCenter1, cavernCenter2, "open", Equal, 1)
for _, tile in pairs(rightPath) do
  tile.color = {0, 1, 0}
end
