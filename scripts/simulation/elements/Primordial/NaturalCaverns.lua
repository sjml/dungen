maxCaverns = 6
cavernCount = 0
breakRoll = 6
padding = 3

cavernConstraints = {
  InTileRange(
    {padding, gir("GroundLine") + padding},
    {gir("TileWidth") - padding, gir("TileHeight") - padding}
  ),
  HasAllTags("ground"),
  HasAttributes("open", Equal, 0),
  MinDistanceFromAttribute(2, "open", Equal, 1)
}
cavernSolver = ConstraintSolver(cavernConstraints)

while (cavernCount < maxCaverns) do
  -- pick our spot
  cavernSolver:Solve()

  local base = cavernSolver.pickedTile
  if base == nil then
    break
  end

  local cavern = base:GetCircle(1)
  for _, tile in ipairs(cavern) do
    tile:SetAttributeInt("open", 1)
  end

  sir("DieSides", 8)
  push("System.DieRoll")
  local result = gir("DieRollResult")

  if (result == 1) then
    print("tunnel")
  elseif (result == 2) then
    print("plague")
  elseif (result == 3) then
    print("gems")
  elseif (result == 4) then
    print("empty")
  elseif (result == 5) then
    print("monster")
  elseif (result == 6) then
    print("FATE")
  elseif (result == 7) then
    print("water")
  elseif (result == 8) then
    print("magma")
  end

  if (result == breakRoll) then
    break
  end

  cavernCount = cavernCount + 1
end
