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
  local t = nil
  if RandomRangeInt(0,4) == 0 then
    t = storeInChamber(c, "gems")
    if t then
      t:AddTag("gold")
    end
  else
    t = storeInChamber(c, "gold")
  end
  if t ~= nil then
    t:SetAttributeInt("treasure", 1 + t:GetAttributeInt("treasure"))
  end
end

base:AddTag("monster")
