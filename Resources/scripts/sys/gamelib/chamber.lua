function makeChamber(baseTile, radius)
  local cavern = baseTile:GetCircle(radius)
  local chamber = CreateRegion()
  for _, tile in pairs(cavern) do
    tile:SetAttributeInt("open", 1)
    tile:SetAttributeInt("empty", 1)
    chamber:AddTile(tile)
  end
  chamber:AddTag("chamber")
  chamber:AddTag("ground")
  return chamber
end

function storeInChamber(chamber, tag, allowFlood)
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

  if ((#tiles - #empties) >= #tiles // 2) then
    if (allowFlood == nil or allowFlood == false) then
      return nil
    end
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
