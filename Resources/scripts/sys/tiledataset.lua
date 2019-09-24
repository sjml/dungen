local TileDataSet = {
  isSet = true
}
TileDataSet.__index = TileDataSet

function TileDataSet:new(t)
  local set = {}
  for _, el in ipairs(t or {}) do
    set[el.meta.i] = true
  end
  setmetatable(set, TileDataSet)
  return set
end

function TileDataSet:toList()
  local t = {}
  for k, v in pairs(self) do
    table.insert(t, GetTileAtIndex(k))
  end
  return t
end

function TileDataSet:union(other)
  local res = TileDataSet:new()
  for k in pairs(self) do
    res[k] = true
  end
  for k in pairs(other) do
    res[k] = true
  end
  return res
end

function TileDataSet:intersection(other)
  local res = TileDataSet:new()
  for k in pairs(self) do
    res[k] = other[k]
  end
  return res
end

function TileDataSet:__tostring()
  local s = "{"
  local sep = ""
  for e in pairs(self) do
    s = s .. sep .. tostring(e)
    sep = ", "
  end
  return s .. "}"
end

return TileDataSet
