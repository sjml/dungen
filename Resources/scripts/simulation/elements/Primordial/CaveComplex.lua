padding = 3
complexConstraints = {
  InTileRange(
    {padding, gir("GroundLine") + padding},
    {gir("TileWidth") - padding, gir("TileHeight") - padding}
  ),
  HasAttributes("open", Equal, 0),
  MinDistanceFromAttribute(4, "open", Equal, 1),
}

complexSolver = ConstraintSolver(complexConstraints)
complexSolver:Solve()

base = complexSolver.pickedTile

chamber = makeRoundChamber(base, 1)
centers = {base}
chambers = {chamber}

for i=1,2 do
  local caveConstraints = {
    InTileRange(
      {padding, gir("GroundLine") + padding},
      {gir("TileWidth") - padding, gir("TileHeight") - padding}
    ),
    HasAttributes("open", Equal, 0),
    MinDistanceFromAttribute(2, "open", Equal, 1),
    MinDistanceFromTag(2, "sky"),
    MinDistanceFromTile(3, base),
    CloseToTile(base),
  }
  local caveSolver = ConstraintSolver(caveConstraints)
  caveSolver:Solve()
  table.insert(centers, caveSolver.favoriteTile)
  table.insert(chambers, makeRoundChamber(caveSolver.favoriteTile, 1))
end

str("TunnelStart", base)
for i=2,3 do
  str("TunnelEnd", centers[i])
  push("System.Tunnel")
end

for _, t in pairs(centers) do
  sir("MonsterCount", gir("MonsterCount") + 1)
  t:AddTag("monster")
end
