-- ENCOUNTERS
  -- if it finds sky:
    -- make a lake and die

  -- how does it react when it finds already-open space that is not a chamber?
    -- i.e. earthquake path, magma chimney, mine shaft, etc.

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




local flowPath = {}
local washingTags = {}


function carveOut(agent, targetTile)
  targetTile:SetAttributeInt("open", 1)
  targetTile:SetAttributeInt("carvedBy", agent.i)
  agent.domain:AddTile(targetTile)
end



function addWashedTags(tile)
  for k,v in pairs(washingTags) do print(k,v) end
  for tag, duration in pairs(washingTags) do
    tile:AddTag(tag)
    if duration == 1 then
      washingTags[tag] = nil
    else
      washingTags[tag] = duration - 1
    end
  end
end



function addWater(tile)
  if tile:HasTags("water") ~= true then
    tile:AddTag("water")
    addWashedTags(tile)
    table.insert(flowPath, tile)
    return true
  end

  local preferenceOrder = {SOUTHEAST, SOUTHWEST, EAST, WEST}
  local secondaryPrefs  = {NORTHEAST, NORTHWEST}

  -- so it meanders down as it falls
  if #flowPath >= 2
     and flowPath[#flowPath - 1]:GetNeighbor(SOUTHEAST) == flowPath[#flowPath] then
    preferenceOrder = {SOUTHWEST, SOUTHEAST, EAST, WEST}
  end


  local backFlow = 7

  local nextFlow = nil
  local flowCheckIdx = #flowPath
  while nextFlow == nil and flowCheckIdx > 0 and flowCheckIdx > #flowPath - backFlow do
    local tc = flowPath[flowCheckIdx]
    for _, dir in ipairs(preferenceOrder) do
      local nt = tc:GetNeighbor(dir)
      if nt and nt:HasTags("water") ~= true and nt:GetAttributeInt("open") == 1 then
        nextFlow = nt
        break
      end
    end
    flowCheckIdx = flowCheckIdx - 1
    local tcc = getChamber(tc)
    if tcc ~= nil and tcc ~= getChamber(flowPath[#flowPath]) then
      nextFlow = nil
      break
    end
  end

  if nextFlow ~= nil then
    nextFlow:AddTag("water")
    addWashedTags(nextFlow)
    table.insert(flowPath, nextFlow)
    return true
  end

  local flowCheckIdx = #flowPath
  while nextFlow == nil and flowCheckIdx > 0 and flowCheckIdx > #flowPath - backFlow do
    local tc = flowPath[flowCheckIdx]
    for _, dir in ipairs(secondaryPrefs) do
      local nt = tc:GetNeighbor(dir)
      if nt and nt:HasTags("water") ~= true and nt:GetAttributeInt("open") == 1 then
        nextFlow = nt
        break
      end
    end
    flowCheckIdx = flowCheckIdx - 1
    local tcc = getChamber(tc)
    if tcc ~= nil and tcc ~= getChamber(flowPath[#flowPath]) then
      nextFlow = nil
      break
    end
  end

  if nextFlow ~= nil then
    nextFlow:AddTag("water")
    addWashedTags(tile)
    table.insert(flowPath, nextFlow)
    return true
  end

  return false
end



function checkForEncounters(agent, targetTile, originTile, agentsMet, chambersMet)
  local agentList = agentsMet or {}
  local chamberList = chambersMet or {}
  local numEncounters = 0

  local owningAgent = targetTile:GetOwner()
  if owningAgent ~= nil and owningAgent ~= agent then
    if agentList[owningAgent.i] == nil then
      agentList[owningAgent.i] = 1
      -- we found a new friend!
        -- resolve encounter with other agent, increment numEncounters
        -- if this agent is now dead, return nil, numEncounters
    end
  end

  local exitPoint = targetTile

  local chamber = getChamber(targetTile)
  if chamber ~= nil then
    if chamberList[chamber.i] == nil then
      chamberList[chamber.i] = 1
      -- print("entered chamber")
      numEncounters = numEncounters + 1

      local accessPoints = getChamberAccesses(chamber)
      for i, t_d in ipairs(accessPoints) do
        if t_d[1] == originTile then
          table.remove(accessPoints, i)
        end
      end

      if #accessPoints == 0 then
        local ingressDir = -1
        for _, dir in pairs({WEST, NORTHWEST, NORTHEAST, EAST, SOUTHEAST, SOUTHWEST}) do
          if originTile:GetNeighbor(dir) == targetTile then
            ingressDir = dir
            break
          end
        end

        local cFlow = targetTile
        while cFlow ~= nil and IsTileInSet(chamber.tiles, cFlow) do
          cFlow = cFlow:GetNeighbor(ingressDir)
        end
        if cFlow ~= nil then
          carveOut(agent, cFlow)
          table.insert(accessPoints, {cFlow, ingressDir})
        end
      end

      local egressPoint = nil
      while egressPoint == nil and addWater(targetTile) do
        local last = flowPath[#flowPath]
        AddTileToRegion(agent.domain, last)
        for _, t_d in ipairs(accessPoints) do
          if t_d[1] == last then
            egressPoint = t_d
            break
          end
        end
      end

      if egressPoint == nil then
        -- can this still happen? maybe if we set it so not to carve egress sometimes...
      else
        exitPoint = egressPoint[1]
      end

      if chamber:HasTags("plague") then
        sir("DieSides", 4)
        push("System.DieRoll")
        washingTags["plague"] = gir("DieRollResult") + 8
      end
    end
  else
    if targetTile:HasTags("gold") then
      targetTile:RemoveTag("gold")
      sir("DieSides", 4)
      push("System.DieRoll")
      washingTags["gold"] = gir("DieRollResult") + 1
    end
  end

  return exitPoint, numEncounters
end



function exploreTile(agent, targetTile, originTile, isCarving)
  if targetTile:GetAttributeInt("open") == 0 then
    if isCarving ~= true then
      return nil
    end

    -- first figure out which of the targets neighbors we
    --    *can't* currently path to
    local neighbors = targetTile:GetNeighbors()
    for i, n in ipairs(neighbors) do
      if originTile:HasOpenPathTo(n) then
        table.remove(neighbors, i)
      elseif originTile == n then
        table.remove(neighbors, i)
      end
    end

    -- sort table to check tiles along the current flow path first
      -- (means chamber resolution has a better sense of how we entered)
    local flowDir = originTile:GetNeighborDirection(targetTile)
    table.sort(neighbors, function(t1, t2)
      if targetTile:GetNeighborDirection(t1) == flowDir
        and targetTile:GetNeighborDirection(t2) ~= flowDir
        then
          return true
      else
        return false
      end
    end)

    local continuations = {}
    local numEncounters = 0

    -- make a hole
    carveOut(agent, targetTile)
    local cont, encs = checkForEncounters(agent, targetTile, originTile)
    addWater(targetTile)
    if cont ~= nil then
      continuations[cont] = 1
    end
    numEncounters = numEncounters + encs
    local tileEncs = numEncounters

    local agentsMet = {}
    local chambersMet = {}
    -- which can we get to *now*
    for i, n in ipairs(neighbors) do
      if originTile:HasOpenPathTo(n) then
        local cont, encs = checkForEncounters(agent, n, targetTile, agentsMet, chambersMet)
        numEncounters = numEncounters + encs
        if cont == nil then
          agent:SetAttributeInt("alive", 0)
          break
        else
          continuations[cont] = 1
        end
      end
    end

    if numEncounters == 0 then
      return targetTile
    else
      if numEncounters ~= tileEncs then
        table.remove(continuations, 1)
      end
      -- TODO: get all the possibilities and pick one randomly?
      for tile, _ in pairs(continuations) do
        return tile
      end
    end
  else
    -- already open, so just check this one
    local cont, encs = checkForEncounters(agent, targetTile, originTile)
    return cont
  end
end



function explorePath(agent, startTile, path, isCarving)
  local curr = startTile
  for i, dir in ipairs(path) do
    local next = curr:GetNeighbor(dir)
    if next == nil then
      agent:SetAttributeInt("alive", 0)
      return nil
    end

    curr = exploreTile(agent, next, curr, isCarving)
    if   curr == nil  -- couldn't find a next tile
      or curr ~= next -- the next tile isn't what we expected; probably had an encounter
      then
        break
    end
  end
  return curr
end



function reflowRiver(agent)
  -- print("reflowing...")
  local src = GetTileAtIndex(agent:GetAttributeInt("riverSource"))
  for _, t in pairs(GetTilesFromSet(agent.domain.tiles)) do
    agent.domain:RemoveTile(t)
    t:RemoveTag("water")
  end

  flowPath = {}
  washingTags = {}
  local flowing = true
  while flowing do
    flowing = addWater(src)

    local prev  = flowPath[#flowPath - 1]
    local mouth = flowPath[#flowPath]
    if prev ~= nil and mouth ~= nil then
      mouth = checkForEncounters(agent, mouth, prev)
    end

    if #flowPath %3 == 0 then
      -- push()
    end
  end

  for _, t in pairs(flowPath) do
    agent.domain:AddTile(t)
  end
end




source = GetTileAtPosition(0, 22)

riverAgent = CreateAgent()
riverAgent:SetAttributeInt("alive", 1)
riverAgent:SetAttributeString("name", "UndergroundRiver" .. tostring(gir("RiverCount")))
riverAgent:SetAttributeString("type", "Natural")
riverAgent:SetAttributeString("class", "Water")
riverAgent:SetAttributeString("alignment", "Indifferent")

riverAgent:SetAttributeInt("riverSource", source.i)
carveOut(riverAgent, source)
addWater(source)

local curr = source
while (riverAgent:GetAttributeInt("alive") ~= 0) do
  sir("DieSides", 10)
  push("System.DieRoll")

  local path = {}
  local result = gir("DieRollResult")

  if (result >= 1 and result <= 3) then
    -- move horizontally
    path = {EAST, EAST, EAST, EAST, EAST}
    curr = explorePath(riverAgent, curr, path, true)
  elseif (result >= 4 and result <= 5) then
    -- angle up
    path = {NORTHEAST, NORTHEAST, NORTHEAST, NORTHEAST, NORTHEAST}
    curr = explorePath(riverAgent, curr, path, true)
  elseif (result >= 6 and result <= 7) then
    -- angle down
    path = {SOUTHEAST, SOUTHEAST, SOUTHEAST, SOUTHEAST, SOUTHEAST}
    curr = explorePath(riverAgent, curr, path, true)
  elseif (result >= 8 and result <= 9) then
    -- move horizontally and form a cave
    local cavernBase = curr:GetNeighbor(EAST)
    if cavernBase then cavernBase = cavernBase:GetNeighbor(EAST) end
    if cavernBase then
      makeRoundChamber(cavernBase, 1, riverAgent)
      curr = explorePath(riverAgent, curr, {EAST, EAST}, true)
      curr = explorePath(riverAgent, curr, {EAST, EAST}, true)
    else
      curr = explorePath(riverAgent, curr, {EAST, EAST, EAST}, true)
    end
  else
    -- waterfall
    path = {
      EAST,
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
    local egress = chamberTiles[#chamberTiles]:GetNeighbor(EAST)
    if egress ~= nil then
      carveOut(riverAgent, egress)
    end
    curr = explorePath(riverAgent, curr, {path[1], path[2]}, true)
  end

  if curr == nil then
    riverAgent:SetAttributeInt("alive", 0)
  end
end

reflowRiver(riverAgent)
