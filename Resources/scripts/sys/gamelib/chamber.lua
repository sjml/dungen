function makeChamber(baseTile, radius, agent)
  local cavern = baseTile:GetCircle(radius)
  return makeChamberFromList(cavern, agent)
end

function makeChamberFromList(tileList, agent)
  local chamber = CreateRegion()
  for _, tile in pairs(tileList) do
    tile:SetAttributeInt("open", 1)
    tile:SetAttributeInt("empty", 1)
    if agent ~= nil then
      tile:SetAttributeInt("carvedBy", agent.i)
      agent.domain:AddTile(tile)
      chamber:SetParent(agent.domain)
    end
    chamber:AddTile(tile)
  end
  chamber:AddTag("chamber")
  chamber:AddTag("ground")
  return chamber
end

function getChamber(tile)
  local regs = tile.memberRegions
  while #regs > 0 do
    local reg = table.remove(regs)
    if reg:HasTags("chamber") then
      return reg
    end
    table.insert(regs, reg.parent) -- might be nil, but ok
  end
  return nil
end

-- for things like water and magma
function getFloodTile(chamber, tag, source)
  local tiles = GetTilesFromSet(chamber.tiles)
  local notFlooded = {}
  for _, t in pairs(tiles) do
    if t:HasTags(tag) ~= true then
      table.insert(notFlooded, t)
    end
  end

  if #notFlooded == 0 then
    return nil
  end

  -- find lowest tile *without* this tag (since it can overlap)
  local lowY = -1
  local xVals = {}
  local lowTile = nil
  for _, t in pairs(notFlooded) do
    if t.hexPos.y > lowY then
      xVals = {}
      table.insert(xVals, t.hexPos.x)
      lowY = t.hexPos.y
      lowTile = t
    elseif t.hexPos.y == lowY then
      table.insert(xVals, t.hexPos.x)
    end
  end
  -- pick the one nearest the source of the flow
  for _, x in pairs(xVals) do
    if math.abs(x - source.hexPos.x) < math.abs(lowTile.hexPos.x - source.hexPos.x) then
      lowTile = GetTileAtPosition(x, lowY)
    end
  end

  return lowTile
end

-- for things like gold, food, people
function storeInChamber(chamber, tag)
  local tiles = GetTilesFromSet(chamber.tiles)
  local empties = {}
  for _, t in pairs(tiles) do
    if t:GetAttributeInt("empty") == 1 then
      table.insert(empties, t)
    end
  end

  if (#empties == 0) then
    return nil
  end

  local lowY = -1
  local xVals = {}
  local lowTile = nil
  for _, tile in pairs(empties) do
    if tile.hexPos.y > lowY then
      xVals = {}
      table.insert(xVals, tile.hexPos.x)
      lowY = tile.hexPos.y
      lowTile = tile
    elseif tile.hexPos.y == lowY then
      table.insert(xVals, tile.hexPos.x)
    end
  end
  table.sort(xVals)
  local xCoord = xVals[math.ceil(#xVals / 2)]
  local lowTile = GetTileAtPosition(xCoord, lowY)

  lowTile:SetAttributeInt("empty", 0)
  lowTile:AddTag(tag)

  return lowTile
end
