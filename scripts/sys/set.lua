local Set = {
  isSet = true
}
Set.__index = Set

function Set:new(t)
  local set = {}
  for _, el in ipairs(t or {}) do
    set[el] = true
  end
  setmetatable(set, Set)
  return set
end

function Set:toList()
  local t = {}
  for k, v in pairs(self) do
    table.insert(t, k)
  end
  return t
end

function Set:union(other)
  local res = Set:new()
  for k in pairs(self) do
    res[k] = true
  end
  for k in pairs(other) do
    res[k] = true
  end
  return res
end

function Set:intersection(other)
  local res = Set:new()
  for k in pairs(self) do
    res[k] = other[k]
  end
  return res
end

function Set:__tostring()
  local s = "{"
  local sep = ""
  for e in pairs(self) do
    s = s .. sep .. tostring(e)
    sep = ", "
  end
  return s .. "}"
end

return Set
