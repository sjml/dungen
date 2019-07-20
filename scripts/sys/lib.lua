local sims = {}

function loadFiles(dir)
  for file in lfs.dir(dir) do
    if string.find(file, ".lua$") then
      local simName = file:sub(1, -5)
      local f, err = loadfile(dir .. "/" .. file)
      if (f ~= nil) then
        sims[simName] = f
      else
        io.stderr:write("LUA ERROR: Couldn't load code for " .. simName .. ":")
        io.stderr:write(err)
        io.stderr:write("\n")
      end
    end
  end

  if (sims["Null"] == nil) then
    sims["Null"] = function() end
  end
end

function VM_wrap(sim)
  local f = sims[sim]
  if (f == nil) then
    io.stderr:write("LUA WARNING: No element called " .. sim .. ".\n")
    f = sims["Null"]
  end

  return coroutine.wrap(f)
end

function table.slice(tbl, first, last, step)
  local sliced = {}

  for i = first or 1, last or #tbl, step or 1 do
    sliced[#sliced+1] = tbl[i]
  end

  return sliced
end

function countInString(s1, s2)
  return select(2, s1:gsub(s2, ""))
end

function push(className)
  if (className == nil) then
    className = "Null"
  end

  sim = CreateSimulationElement(className)
  HLVMPush(sim)

  local f, r = coroutine.running()
  if (r ~= true) then
    --  TODO: this is a hack because the C code is not
    --        calling sim elements as true coroutines
    --        because of reasons
    coroutine.yield(1)
  end
end

function getfntable(classname)
  local reg = debug.getregistry()
  local classTable = reg["SWIG"][classname]
  if (classTable == nil) then
    io.stderr:write("LUA WARNING: No class called " .. classname .. ".\n")
    return
  end
  return classTable[".fn"]
end

tdft = getfntable("TileData")
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


tsft = getfntable("TileSet")
tsft.SetAttributeInt = function(ts, name, value)
  SetTileSetAttributeInt(ts, name, value)
end

tsft.SetAttributeFloat = function(ts, name, value)
  SetTileSetAttributeFloat(ts, name, value)
end

tsft.SetAttributeString = function(ts, name, value)
  SetTileSetAttributeString(ts, name, value)
end

tsft.GetAttributeInt = function(ts, name)
  return GetTileSetAttributeInt(ts, name)
end

tsft.GetAttributeFloat = function(ts, name)
  return GetTileSetAttributeFloat(ts, name)
end

tsft.GetAttributeString = function(ts, name)
  return GetTileSetAttributeString(ts, name)
end

tsft.AddTag = function(ts, tag)
  AddTileSetTag(ts, tag)
end

tsft.RemoveTag = function(ts, tag)
  RemoveTileSetTag(ts, tag)
end

tsft.HasTags = function(ts, tagStr)
  return TileSetHasTags(ts, tagStr)
end

tsft.GetTags = function(ts)
  return GetTileSetTags(ts)
end

tsft.AddTile = function(ts, t)
  return AddTileToSet(ts, t)
end

tsft.RemoveTile = function(ts, t)
  return RemoveTileFromSet(ts, t)
end

tsft.HasTile = function(ts, t)
  return IsTileInSetFromSet(ts, t)
end

tsft.GetTileCount = function(ts)
  return GetTileCountFromSet(ts)
end

tsft.GetTiles = function(ts)
  return GetTiles(ts)
end

tsft.SetOutline = function(ts, c)
  return SetTileSetOutline(ts, c)
end

tsft.ClearOutline = function(ts)
  return ClearTileSetOutline(ts)
end
