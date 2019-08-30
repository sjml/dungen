sir("MonsterCount", gir("MonsterCount") + 1)

padding = 3
wyrmConstraints = {
  InTileRange(
    {padding, gir("GroundLine") + padding},
    {gir("TileWidth") - padding, gir("TileHeight") - padding}
  ),
  HasAttributes("open", Equal, 0),
  MinDistanceFromAttribute(3, "open", Equal, 1),
  MinDistanceFromTag(4, "sky"),
}

wyrmSolver = ConstraintSolver(wyrmConstraints)
wyrmSolver:Solve()

base = wyrmSolver.pickedTile
c = makeChamber(base, 2)
c:AddTag("GreatWyrm")

for i=1,5 do
  if RandomRangeInt(0,4) == 0 then
    local t = storeInChamber(c, "gems")
    if t then
      t:AddTag("gold")
    end
  else
    storeInChamber(c, "gold")
  end
end

base:AddTag("monster")
