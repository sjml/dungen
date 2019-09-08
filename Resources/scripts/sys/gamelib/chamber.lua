function makeRoundChamber(baseTile, radius, agent)
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

function getChamberAccesses(chamber)
  local accessIdxs = {}

  for _, t in pairs(GetTilesFromSet(chamber.tiles)) do
    for _, dir in pairs({WEST, NORTHWEST, NORTHEAST, EAST, SOUTHEAST, SOUTHWEST}) do
      local n = t:GetNeighbor(dir)
      if n ~= nil then
        if IsTileInSet(chamber.tiles, n) ~= true and
           n:GetAttributeInt("open") == 1 then
            accessIdxs[n.i] = dir
        end
      end
    end
  end

  local accessTiles = {}
  for idx, dir in pairs(accessIdxs) do
    table.insert(accessTiles, {GetTileAtIndex(idx), dir})
  end

  return accessTiles
end

function storeItemInChamber(chamber, tag)
  local cap = chamber:GetAttributeInt("chamberStorageAmount")
  chamber:SetAttributeInt("chamberStorageAmount", cap + 1)
  local sAttr = "chamberStorage" .. tostring(cap + 1)
  chamber:SetAttributeString(sAttr, tag)

  -- TODO: this function should be the one to actually
  --   check if there's room, instead of the display reset
  return _resetStorageDisplay(chamber)
end

function removeItemFromChamber(chamber, tag)
  local cap = chamber:GetAttributeInt("chamberStorageAmount")

  local found = false
  for i=1,cap do
    local sAttr = "chamberStorage" .. tostring(i)
    if found then
      local prev = "chamberStorage" .. tostring(i-1)
      chamber:SetAttributeString(prev, chamber:GetAttributeString(sAttr))
    else
      if chamber:GetAttributeString(sAttr) == tag then
        found = true
      end
    end
  end

  if found then
    chamber:SetAttributeInt("chamberStorageAmount", cap - 1)
    _resetStorageDisplay(chamber)
    return true
  else
    return false
  end
end

function getItemsInChamber(chamber)
  local ret = {}
  local cap = chamber:GetAttributeInt("chamberStorageAmount")
  for i=1,cap do
    table.insert(ret, chamber:GetAttributeString("chamberStorage" .. tostring(i)))
  end
  return ret
end

function _resetStorageDisplay(chamber)
  -- clear out all storage tags
  local chamberTiles = GetTilesFromSet(chamber.tiles)
  for _, tile in pairs(chamberTiles) do
    local tags = tile:GetTags()
    for _, tag in pairs(tags) do
      if tag:starts("storage_") then
        tile:RemoveTag(tag)
        tile:RemoveTag(tag:sub(string.len("storage_")+1, -1))
        tile:SetAttributeInt("open", 1)
      end
    end
  end

  local cap = chamber:GetAttributeInt("chamberStorageAmount")
  for i=1,cap do
    local tag = chamber:GetAttributeString("chamberStorage" .. tostring(i))
    local sTag = "storage_" .. tag
    local empties = {}
    for _, tile in pairs(chamberTiles) do
      local tTags = tile:GetTags()
      local open = true
      for _, tag in pairs(tTags) do
        if tag:starts("storage_") then
          open = false
          break
        end
      end
      if open then
        table.insert(empties, tile)
      end
    end

    if #empties == 0 then
      io.stderr:write("LUA ERROR: Couldn't fit `"..tag.."` in storage chamber.\n")
      return false
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

    lowTile:AddTag(tag)
    lowTile:AddTag(sTag)
    lowTile:SetAttributeInt("open", 0)
  end

  return true
end
