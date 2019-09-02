local TileDataSet = require("sys.tiledataset")

local tdft = GetFunctionTable("TileData")
tdft.GetCircle = function(td, radius)
  return GetTileCircle(td, radius)
end

tdft.GetNeighbors = function(td)
  return GetTileCircle(td, 1)
end

WEST = 1
NORTHWEST = 2
NORTHEAST = 3
EAST = 4
SOUTHEAST = 5
SOUTHWEST = 6

tdft.GetNeighbor = function(td, direction)
  local ret = nil
  if     direction == WEST then
    ret = GetTileAtIndex(td.neighborW)
  elseif direction == NORTHWEST then
    ret = GetTileAtIndex(td.neighborNW)
  elseif direction == NORTHEAST then
    ret = GetTileAtIndex(td.neighborNE)
  elseif direction == EAST then
    ret = GetTileAtIndex(td.neighborE)
  elseif direction == SOUTHEAST then
    ret = GetTileAtIndex(td.neighborSE)
  elseif direction == SOUTHWEST then
    ret = GetTileAtIndex(td.neighborSW)
  end
  return ret
end

tdft.BuildPath = function(td, dirList)
  local ret = {}
  local curr = td
  for _, d in pairs(dirList) do
    local nt = curr:GetNeighbor(d)
    if nt == nil then
      break
    end
    table.insert(ret, nt)
    curr = nt
  end
  return ret
end

tdft.SetAttributeInt = function(td, name, value)
  SetTileAttributeInt(td, name, value)
end

tdft.SetAttributeFloat = function(td, name, value)
  SetTileAttributeFloat(td, name, value)
end

tdft.SetAttributeString = function(td, name, value)
  SetTileAttributeString(td, name, value)
end

tdft.GetAttributeInt = function(td, name)
  return GetTileAttributeInt(td, name)
end

tdft.GetAttributeFloat = function(td, name)
  return GetTileAttributeFloat(td, name)
end

tdft.GetAttributeString = function(td, name)
  return GetTileAttributeString(td, name)
end

tdft.CheckAttribute = function(td, name, comp, value)
  return CheckTileAttribute(td, name, comp, value)
end

tdft.AddTag = function(td, tag)
  AddTileTag(td, tag)
end

tdft.RemoveTag = function(td, tag)
  RemoveTileTag(td, tag)
end

tdft.HasTags = function(td, tagStr)
  return TileHasTags(td, tagStr)
end

tdft.GetTags = function(td)
  return GetTileTags(td)
end

tdft.SetOutline = function(td, color, thickness, type)
  return SetTileOutline(td, color, thickness, type)
end

tdft.ClearOutline = function(td)
  return ClearTileOutline(td)
end
