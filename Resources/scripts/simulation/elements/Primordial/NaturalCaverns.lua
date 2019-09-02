maxCaverns = gir("NaturalCavernCreationCount")
cavernCount = 0
breakRoll = 6
padding = 3


while (cavernCount < maxCaverns) do
  -- pick our spot
  local cavernConstraints = {
    InTileRange(
      {padding, gir("GroundLine") + padding},
      {gir("TileWidth") - padding, gir("TileHeight") - padding}
    ),
    HasAttributes("open", Equal, 0),
    MinDistanceFromAttribute(2, "open", Equal, 1)
  }
  local cavernSolver = ConstraintSolver(cavernConstraints)
  cavernSolver:Solve()

  local base = cavernSolver.pickedTile
  if base == nil then
    break
  end

  chamber = makeChamber(base, 1)
  chamber:AddTag("natural")

  sir("DieSides", 8)
  push("System.DieRoll")
  local result = gir("DieRollResult")

  if (result == 1) then
    -- tunnel
    local tunnel = base:BuildPath({WEST, WEST, EAST, EAST, EAST, EAST})
    for _, tunnelTile in pairs(tunnel) do
      tunnelTile:SetAttributeInt("open", 1)
    end
  elseif (result == 2) then
    -- plague
    sir("DieSides", 4)
    push("System.DieRoll")
    chamber:SetAttributeInt("plagueStrength", gir("DieRollResult"))
    chamber:AddTag("plague")
  elseif (result == 3) then
    -- gems
    sir("DieSides", 4)
    push("System.DieRoll")
    for i=1,gir("DieRollResult") do
      storeInChamber(chamber, "gems", true)
    end
    chamber:SetAttributeInt("treasure", gir("DieRollResult") + chamber:GetAttributeInt("treasure"))
    -- chamber:AddTag("gems")
  elseif (result == 4) then
    -- empty
  elseif (result == 5) then
    -- wandering monster
    sir("MonsterCount", gir("MonsterCount") + 1)
    base:AddTag("monster")
  elseif (result == 6) then
    -- FATE
    chamber:AddTag("FATE")
  elseif (result == 7) then
    -- water
    chamber:AddTag("water")
    local stillSpace = true
    while (stillSpace) do
      stillSpace = storeInChamber(chamber, "water", true)
    end
  elseif (result == 8) then
    -- magma
    chamber:AddTag("magma")
    local stillSpace = true
    while (stillSpace) do
      stillSpace = storeInChamber(chamber, "magma", true)
    end
  end

  if (result == breakRoll) then
    break
  end

  cavernCount = cavernCount + 1
end
