-- FIRST: check logic for when it hits the sky; somehow it made a cavern up there?

-- ENCOUNTERS
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

-- NOTE: when picking starting point, make sure to pick a tile that is currently closed

sir("RiverCount", gir("RiverCount") + 1)

riverAgent = CreateAgent()
riverAgent:SetAttributeInt("alive", 1)
riverAgent:SetAttributeString("name", "UndergroundRiver" .. tostring(gir("RiverCount")))
riverAgent:SetAttributeString("type", "Natural")
riverAgent:SetAttributeString("class", "Water")
riverAgent:SetAttributeString("alignment", "Indifferent")

function carveOut(agent, targetTile)
  targetTile:SetAttributeInt("open", 1)
  targetTile:SetAttributeInt("carvedBy", agent.i)
  agent.domain:AddTile(targetTile)
end

local riverIsFlowing = false
local washingTags = {}

function checkForEncounters(agent, targetTile, fromTile)
  if riverIsFlowing ~= true then
    if targetTile:HasTags("sky") then
      agent:SetAttributeInt("alive", 0)
      local lake = {fromTile}
      local t = fromTile:GetNeighbor(EAST)
      table.insert(lake, t)
      t = t:GetNeighbor(SOUTHWEST)
      table.insert(lake, t)
      t = t:GetNeighbor(EAST)
      table.insert(lake, t)
      t = t:GetNeighbor(NORTHEAST)
      table.insert(lake, t)
      for _, t in pairs(lake) do
        carveOut(agent, t)
      end
      return nil
    end
  else
    -- this stuff only gets processed if the river is flowing
    if targetTile:HasTags("gold") then
      sir("DieSides", 4)
      push("System.DieRoll")
      washingTags["gold"] = gir("DieRollResult") + 1
    end
    if targetTile:HasTags("gems") then
      sir("DieSides", 2)
      push("System.DieRoll")
      washingTags["gems"] = gir("DieRollResult") + 1
    end

    local chamber = getChamber(targetTile)

    if targetTile:HasTags("plague") or (chamber ~= nil and chamber:HasTags("plague")) then
      sir("DieSides", 12)
      push("System.DieRoll")
      washingTags["plague"] = gir("DieRollResult")
    end
  end

  return targetTile
end

function exploreTile(agent, targetTile, fromTile)
  local encounterNext = nil
  if targetTile:GetAttributeInt("open") == 1 then
    encounterNext = checkForEncounters(agent, targetTile, fromTile)
  else
    carveOut(agent, targetTile)
    local neighbors = targetTile:GetNeighbors()
    local encounterNext = nil
    for _, n in pairs(neighbors) do
      if agent:GetAttributeInt("alive") == 0 then
        break
      end
      if n ~= fromTile then
        if n:GetAttributeInt("open") == 1 then
          encounterNext = checkForEncounters(agent, n, targetTile)
        end
      end
    end
  end

  if encounterNext == nil then
    return targetTile
  else
    return encounterNext
  end
end

function explorePath(agent, startTile, path)
  local curr = startTile
  for i, dir in ipairs(path) do
    if agent:GetAttributeInt("alive") == 0 then
      return nil
    end
    local next = curr:GetNeighbor(dir)
    if next == nil then
      agent:SetAttributeInt("alive", 0)
      return nil
    end
    curr = exploreTile(agent, next, curr)
    if curr == nil then
      break
    end
  end
  return curr
end

function addWater(tile, agent, flowPath)
  if tile:HasTags("water") ~= true then
    tile:AddTag("water")
    table.insert(flowPath, tile)
    return true
  end

  local preferenceOrder = {SOUTHEAST, SOUTHWEST, EAST, WEST}

  if #flowPath >= 2
     and flowPath[#flowPath - 1]:GetNeighbor(SOUTHEAST) == flowPath[#flowPath] then
    preferenceOrder = {SOUTHWEST, SOUTHEAST, EAST, WEST}
  end

  local secondaryPrefs  = {NORTHEAST, NORTHWEST}

  local nextFlow = nil
  local flowCheckIdx = #flowPath
  while nextFlow == nil and flowCheckIdx > 0 and flowCheckIdx > #flowPath - 4 do
    local tc = flowPath[flowCheckIdx]
    for _, dir in ipairs(preferenceOrder) do
      local nt = tc:GetNeighbor(dir)
      if nt and nt:HasTags("water") ~= true and nt:GetAttributeInt("open") == 1 then
        nextFlow = nt
        break
      end
    end
    flowCheckIdx = flowCheckIdx - 1
  end

  if nextFlow ~= nil then
    nextFlow:AddTag("water")
    table.insert(flowPath, nextFlow)
    return true
  end

  local flowCheckIdx = #flowPath
  while nextFlow == nil and flowCheckIdx > 0 and flowCheckIdx > #flowPath - 4 do
    local tc = flowPath[flowCheckIdx]
    for _, dir in ipairs(secondaryPrefs) do
      local nt = tc:GetNeighbor(dir)
      if nt and nt:HasTags("water") ~= true and nt:GetAttributeInt("open") == 1 then
        nextFlow = nt
        break
      end
    end
    flowCheckIdx = flowCheckIdx - 1
  end

  if nextFlow ~= nil then
    nextFlow:AddTag("water")
    table.insert(flowPath, nextFlow)
    return true
  end

  return false
end


local source = GetTileAtPosition(0, 22)

local curr = source
exploreTile(riverAgent, curr)

while (riverAgent:GetAttributeInt("alive") ~= 0) do
  sir("DieSides", 10)
  push("System.DieRoll")

  local path = {}
  local result = 1 -- gir("DieRollResult")

  if (result >= 1 and result <= 3) then
    -- move horizontally
    path = {EAST, EAST, EAST, EAST, EAST}
    curr = explorePath(riverAgent, curr, path)
  elseif (result >= 4 and result <= 5) then
    -- angle up
    path = {NORTHEAST, NORTHEAST, NORTHEAST, NORTHEAST, NORTHEAST}
    curr = explorePath(riverAgent, curr, path)
  elseif (result >= 6 and result <= 7) then
    -- angle down
    path = {SOUTHEAST, SOUTHEAST, SOUTHEAST, SOUTHEAST, SOUTHEAST}
    curr = explorePath(riverAgent, curr, path)
  elseif (result >= 8 and result <= 9) then
    -- move horizontally and form a cave
    path = {EAST, EAST, EAST, EAST, EAST}
    curr = explorePath(riverAgent, curr, path)
    if curr ~= nil then
      local base = curr:GetNeighbor(WEST):GetNeighbor(WEST)
      local c = makeRoundChamber(base, 1, riverAgent)
    end
  else
    -- waterfall
    path = {
      EAST,
      EAST,
      SOUTHEAST,
      WEST,
      SOUTHWEST,
      EAST,
      EAST,
      SOUTHWEST,
      WEST,
      WEST,
      SOUTHEAST,
      SOUTHEAST,
      NORTHEAST,
      SOUTHEAST,
      NORTHEAST,
      SOUTHEAST
    }
    local chamberTiles = getTilePath(curr, path)
    makeChamberFromList(chamberTiles, riverAgent)
    curr = explorePath(riverAgent, curr, path)
  end

  if curr == nil then
    riverAgent:SetAttributeInt("alive", 0)
  end
end

local riverPath = {}
local treasureTags = {gold = 1, gems = 1}
riverIsFlowing = true
while riverIsFlowing do
  riverIsFlowing = addWater(source, riverAgent, riverPath)

  prev = riverPath[#riverPath - 1]
  mouth = riverPath[#riverPath]
  if prev ~= nil and mouth ~= nil then
    checkForEncounters(riverAgent, mouth, prev)
  end

  for tag, duration in pairs(washingTags) do
    if duration > 0 then
      washingTags[tag] = duration - 1
      mouth:AddTag(tag)
      if treasureTags[tag] == 1 then
        mouth:SetAttributeInt("treasure", 1 + mouth:GetAttributeInt("treasure"))
      end
    end
  end

  if #riverPath % 3 == 0 then
    push()
  end
end
