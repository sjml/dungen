-- kind of trivial right now, but when there are agents/entities,
--   this will also handle triggering encounters, etc.

startTile = gtr("TunnelStart")
endTile = gtr("TunnelEnd")

path = FindSimplePath(startTile, endTile)

for _, tile in pairs(path) do
  tile:SetAttributeInt("open", 1)
end
