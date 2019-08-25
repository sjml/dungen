local TileDataSet = require("sys.tiledataset")

function lazyBasicAssert(item1, item2, desc)
  if (desc == nil) then
    desc = ""
  else
    desc = "("..desc..")"
  end
  if (item1 == item2) then
    print("✅ OK " .. desc)
    return
  elseif (type(item1) ~= "table" and type(item2) == "table") then
    for _, v in pairs(item2) do
      if (item1 == v) then
        print("✅ OK " .. desc)
        return
      end
    end
  end
  io.stderr:write("❌ FAIL! " .. desc .. "\n")
  io.stderr:write("\t" .. tostring(item1) .. " not equal to " .. tostring(item2) .. "\n")
end

function string:split(sep)
  local sep, fields = sep or " ", {}
  local pattern = string.format("([^%s]+)", sep)
  self:gsub(pattern, function(c) fields[#fields+1] = c end)
  return fields
end

function string:starts(startString)
  return string.sub(self, 1, string.len(startString)) == startString
end

function string:ends(endString)
  return endString == "" or string.sub(self, -string.len(endString)) == endString
end

function string:trim()
  local n = self:find"%S"
  return n and self:match(".*%S", n) or ""
end

function countInString(s1, s2)
  return select(2, s1:gsub(s2, ""))
end


function table.slice(tbl, first, last, step)
  local sliced = {}

  for i = first or 1, last or #tbl, step or 1 do
    sliced[#sliced+1] = tbl[i]
  end

  return sliced
end

local function getfntable(classname)
  local reg = debug.getregistry()
  local classTable = reg["SWIG"][classname]
  if (classTable == nil) then
    io.stderr:write("LUA WARNING: No class called " .. classname .. ".\n")
    return
  end
  return classTable[".fn"]
end

local tdft = getfntable("TileData")
tdft.GetNeighbors = function(td)
  local ret = {}
  if (td.neighborW ~= -1) then
    table.insert(ret, GetTileAtIndex(td.neighborW))
  end
  if (td.neighborNW ~= -1) then
    table.insert(ret, GetTileAtIndex(td.neighborNW))
  end
  if (td.neighborNE ~= -1) then
    table.insert(ret, GetTileAtIndex(td.neighborNE))
  end
  if (td.neighborE ~= -1) then
    table.insert(ret, GetTileAtIndex(td.neighborE))
  end
  if (td.neighborSE ~= -1) then
    table.insert(ret, GetTileAtIndex(td.neighborSE))
  end
  if (td.neighborSW ~= -1) then
    table.insert(ret, GetTileAtIndex(td.neighborSW))
  end
  return ret
end

tdft.GetCircle = function(td, radius)
  local allWithDupes = {td}

  local count = 0
  while (count < radius) do
    count = count + 1
    local expansion = {}
    for _, t in ipairs(allWithDupes) do
      for _, n in ipairs(t:GetNeighbors()) do
        table.insert(expansion, n)
      end
    end
    for _, e in ipairs(expansion) do
      table.insert(allWithDupes, e)
    end
  end

  local ts = TileDataSet:new(allWithDupes)
  return ts:toList()
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


local rft = getfntable("Region")
rft.SetAttributeInt = function(r, name, value)
  SetRegionAttributeInt(r, name, value)
end

rft.SetAttributeFloat = function(r, name, value)
  SetRegionAttributeFloat(r, name, value)
end

rft.SetAttributeString = function(r, name, value)
  SetRegionAttributeString(r, name, value)
end

rft.GetAttributeInt = function(r, name)
  return GetRegionAttributeInt(r, name)
end

rft.GetAttributeFloat = function(r, name)
  return GetRegionAttributeFloat(r, name)
end

rft.GetAttributeString = function(r, name)
  return GetRegionAttributeString(ts, name)
end

rft.CheckAttribute = function(r, name, comp, value)
  return false -- TODO: CheckRegionAttribute(td, name, comp, value)
end

rft.AddTag = function(r, tag)
  AddRegionTag(r, tag)
end

rft.RemoveTag = function(r, tag)
  RemoveRegionTag(r, tag)
end

rft.HasTags = function(r, tagStr)
  return RegionHasTags(r, tagStr)
end

rft.GetTags = function(r)
  return GetRegionTags(r)
end

rft.Destroy = function(r)
  return DestroyRegion(r)
end

rft.AddTile = function(r, t)
  return AddTileToRegion(r, t)
end

rft.RemoveTile = function(r, t)
  return RemoveTileFromRegion(r, t)
end

rft.SetOutline = function(r, c, t)
  return SetRegionOutline(r, c, t)
end

rft.ClearOutline = function(r)
  return ClearRegionOutline(r)
end


function gsr(key)
  return GetStringRegister(key)
end

function gir(key)
  return GetIntRegister(key)
end

function gfr(key)
  return GetFloatRegister(key)
end

function gtr(key)
  return GetTileRegister(key)
end

function gtsr(key)
  return GetTileSetRegister(key)
end

function ssr(key, value)
  SetStringRegister(key, value)
end

function sir(key, value)
  SetIntRegister(key, value)
end

function sfr(key, value)
  SetFloatRegister(key, value)
end

function str(key, value)
  SetTileRegister(key, value)
end

function stsr(key, value)
  SetTileSetRegister(key, value)
end
