function carveOut(agent, targetTile)
  targetTile:SetAttributeInt("open", 1)
  targetTile:SetAttributeInt("carvedBy", agent.i)
  agent.domain:AddTile(targetTile)
end


function checkEncountersCENTRAL(agent, targetTile, originTile)
  local ret = {
    agents = {},   -- agents that have been encountered
      -- agentIdx encountered, tile where we encountered them
    chambers = {}, -- chambers that have been encountered
      -- chamberIdx encountered, tile where it was encountered
    tiles = {}, -- tiles (idx) encountered for manual property checks
  }

  local owningAgent = targetTile:GetOwner()
  if owningAgent ~= nil and owningAgent ~= agent then
    if ret.agents[owningAgent.i] == nil then
      ret.agents[owningAgent.i] = targetTile.i
    end
  end

  local chamber = getChamber(targetTile)
  if chamber ~= nil then
    ret.chambers[chamber.i] = targetTile.i
  end

  ret.tiles[targetTile.i] = true

  return ret
end

function _convertEncounterData(data)
  local ret = {
    agents = {},
    chambers = {},
    tiles = {},
  }

  for agentIdx, tileIdx in pairs(data.agents) do
    ret.agents[GetAgentByIndex(agentIdx)] = GetTileAtIndex(tileIdx)
  end
  for chamberIdx, tileIdx in pairs(data.chambers) do
    ret.chambers[GetRegionByIndex(chamberIdx)] = GetTileAtIndex(tileIdx)
  end
  for tileIdx, _ in pairs(data.tiles) do
    table.insert(ret.tiles, GetTileAtIndex(tileIdx))
  end
end

function exploreTileCENTRAL(agent, targetTile, originTile, isCarving)
  if targetTile:GetAttributeInt("open") ~= 0 then
    return _convertEncounterData(checkEncountersCENTRAL(agent, targetTile, originTile))
  elseif isCarving ~= true then
    return nil -- we can't go that way
  end

  -- first figure out which of the neighbors we can't reach before carving
  local neighbors = targetTile:GetNeighbors()
  for i, n in ipairs(neighbors) do
    if originTile == n or originTile:HasOpenPathTo(n) then
      table.remove(neighbors, i)
    end
  end

  -- sort table to check tiles along the current movement path first
    -- (means chamber resolution has a better sense of how we entered)
  local currentDirection = originTile:GetNeighborDirection(targetTile)
  table.sort(neighbors, function(t1, t2)
    if targetTile:GetNeighborDirection(t1) == currentDirection
      and targetTile:GetNeighborDirection(t2) ~= currentDirection
      then
        return true
      else
        return false
      end
  end)

  -- make a hole and see what we find there
  carveOut(agent, targetTile)
  local encounterData = checkEncountersCENTRAL(agent, targetTile, originTile)

  -- and what did we just tunnel into?
  for i, n in ipairs(neighbors) do
    if originTile:HasOpenPathTo(n) then
      local encs = checkEncountersCENTRAL(agent, n, targetTile)
      for agentIdx, tileIdx in pairs(encs.agents) do
        if encounterData.agents[agentIdx] == nil then
          encounterData.agents[agentIdx] = tileIdx
        end
      end
      for chamberIdx, tileIdx in pairs(encs.chambers) do
        if encounterData.chambers[chamberIdx] == nil then
          encounterData.chambers[chamberIdx] = tileIdx
        end
      end
    end
  end

  -- turn encounter data into actual stuff
  return _convertEncounterData(encounterData)
end
