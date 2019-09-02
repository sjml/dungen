
-- does a simple bresenham march, grabbing every tile under the pixels
  -- hella inefficient, but our numbers are small
function getStraightLinePath(startTile, endTile)
  local startWorld = startTile.worldPos
  local endWorld = endTile.worldPos

  local pixStartF = WorldToScreen(startWorld)
  local pixEndF = WorldToScreen(endWorld)

  local pixCurrent = {
    x = math.floor(pixStartF.x + 0.5),
    y = math.floor(pixStartF.y + 0.5),
  }
  local pixEnd = {
    x = math.floor(pixEndF.x + 0.5),
    y = math.floor(pixEndF.y + 0.5),
  }

  local dx = math.abs(pixEnd.x - pixCurrent.x)
  local dy = math.abs(pixEnd.y - pixCurrent.y)
  local sx, sy
  if pixCurrent.x < pixEnd.x then
    sx = 1
  else
    sx = -1
  end
  if pixCurrent.y < pixEnd.y then
    sy = 1
  else
    sy = -1
  end

  local path = {}
  local check = {}
  local err = dx - dy
  repeat
    local t = ScreenToTile({pixCurrent.x, pixCurrent.y})
    if check[t] == nil then
      check[t] = 1
      table.insert(path, t)
    end
    local e2 = 2 * err
    if (e2 > -dy) then
      err = err - dy
      pixCurrent.x = pixCurrent.x + sx
    end
    if (e2 < dx) then
      err = err + dx
      pixCurrent.y = pixCurrent.y + sy
    end
  until (pixCurrent.x == pixEnd.x and pixCurrent.y == pixEnd.y)

  return path
end

function getTilePath(startingTile, directionList)
  if startingTile == nil then
    return {}
  end

  local path = {startingTile}
  local curr = startingTile
  for _, dir in pairs(directionList) do
    curr = curr:GetNeighbor(dir)
    if curr == nil then
      break
    end
    table.insert(path, curr)
  end

  return path
end
