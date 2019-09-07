-- maybe obsolete? moving over to FlowingRiver (which actually models water flow
--    and can also *re*-flow after encountering something, but leaving this here
--    for reference if that proves to be problematic

function waterIt(tile)
  tile:AddTag("water")
  tile:SetAttributeInt("open", 1)
end

function tryPath(current, path)
  for i, direction in ipairs(path) do
    current = current:GetNeighbor(direction)
    if current == nil then
      break
    end
    if current:HasTags("sky") then
      current = current:GetNeighbor(SOUTHEAST)
      waterIt(current:GetNeighbor(SOUTHWEST))
      local j = i
      while j < #path do
        path[j] = SOUTHEAST
        j = j + 1
      end
    end
    waterIt(current)
  end

  return current
end

sir("RiverCount", gir("RiverCount") + 1)

legalSet = TileSet()
for i=gir("GroundLine"), gir("TileHeight") - 1 do
  legalSet = AddTileToSet(legalSet, GetTileAtPosition(0, i))
end
stsr("RandomTileSet", legalSet)
push("System.RandomTile")

startTile = gtr("RandomTile")
DestroyTileSet(legalSet)

current = startTile
waterIt(current)

while (true) do
  if current == nil then
    break
  end

  sir("DieSides", 10)
  push("System.DieRoll")

  local result = gir("DieRollResult")

  if (result >= 1 and result <= 3) then
    -- move horizontally
    path = {EAST, EAST, EAST, EAST, EAST}
    current = tryPath(current, path)
  elseif (result >= 4 and result <= 5) then
    -- angle up
    path = {NORTHEAST, NORTHEAST, NORTHEAST, NORTHEAST, NORTHEAST}
    current = tryPath(current, path)
  elseif (result >= 6 and result <= 7) then
    -- angle down
    path = {SOUTHEAST, SOUTHEAST, SOUTHEAST, SOUTHEAST, SOUTHEAST}
    current = tryPath(current, path)
  elseif (result >= 8 and result <= 9) then
    -- move horizontally and form a cave
    path = {EAST, EAST, EAST, EAST, EAST}
    current = tryPath(current, path)
    if current ~= nil then
      local base = current:GetNeighbor(WEST):GetNeighbor(WEST)
      local c = makeRoundChamber(base, 1)
      for i=1,5 do
        storeInChamber(c, "water", true)
      end
    end
  else
    -- waterfall (half of horizontal length)
    cavernDirs = {EAST, SOUTHEAST, SOUTHWEST, EAST, SOUTHWEST, WEST, WEST, SOUTHEAST, EAST, EAST}
    path = current:BuildPath(cavernDirs)
    for _, t in pairs(path) do
      t:SetAttributeInt("open", 1)
    end

    path = {SOUTHEAST, SOUTHWEST, SOUTHEAST, SOUTHWEST, SOUTHEAST}
    current = tryPath(current, path)
  end
end

