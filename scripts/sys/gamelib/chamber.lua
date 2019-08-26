function storeInChamber(chamber, tag, allowFlood)
  local tiles = GetTilesFromSet(chamber.tiles)
  local empties = {}
  for _, t in pairs(tiles) do
    if t:GetAttributeInt("empty") == 1 then
      table.insert(empties, t)
    end
  end

  if (#empties == 0) then
    return false
  end

  if ((#tiles - #empties) >= #tiles // 2) then
    if (allowFlood == nil or allowFlood == false) then
      return false
    end
  end


  local lowY = -1
  local xVals = {}
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

  return true
end
