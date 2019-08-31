-- ENCOUNTERS
  -- if it encounters sky, end traversal after making a sinkhole/lake

  -- if it finds gold, gems, or plague, wash them downstream for a bit

  -- if it finds magma:
    -- make a chimney upwards and fill with steam

  -- if it finds a chamber, flood it, continue on
    -- if it's a monster's cavern:
      -- if we're in the primordial era, transform the monster to an aquatic one
      -- otherwise, it tries to escape via connected tunnels
        -- if it can't, it dies
    -- if it's a civilization's cavern:
      -- they lose one of whatever population was there; the rest move out

    -- what if it encounters FATE?
      -- ideally: back off and route around it... but that might be hard to manage
      -- more easily: just stop entirely

-- "EXPLORING"
  -- roll the path direction, and move along it
    -- each new tile:
      -- check for encounters
      -- if no encounter:
        -- if it's closed, open it and add water
        -- if it's open, flow the water along open paths until it gets stuck

-- FLOWING RIVER (will need slightly different rules for non-rivers flowing)
  -- tries to flow down (soutwest/southeast; if both, pick one randomly)
  -- if it can't flow down, tries to flow right, then left
  -- if it can't flow:
    -- if not in a chamber, carve chamber
    -- fill the chamber and continue out to the east

-- REFLOWING RIVER
  -- probably mostly follows rules for exploring, but needs to try and follow its old path until it can't

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

riverAgent = CreateAgent()
riverAgent:SetAttributeString("name", "UndergroundRiver" .. tostring(gir("RiverCount")))
riverAgent:SetAttributeString("Type", "Natural")
riverAgent:SetAttributeString("class", "Water")
riverAgent:SetAttributeString("alignment", "Indifferent")

legalSet = TileSet()
for i=gir("GroundLine"), gir("TileHeight") - 1 do
  legalSet = AddTileToSet(legalSet, GetTileAtPosition(0, i))
end
stsr("RandomTileSet", legalSet)
push("System.RandomTile")

startTile = gtr("RandomTile")
DestroyTileSet(legalSet)

startTile = GetTileAtPosition(0, 22)
current = startTile
waterIt(current)

while (true) do
  if current == nil then
    break
  end

  sir("DieSides", 10)
  push("System.DieRoll")

  local result = gir("DieRollResult")
  result = 1

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
      local c = makeChamber(base, 1)
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

