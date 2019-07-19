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
  SetAttributeInt(td, name, value)
end

tdft.SetAttributeFloat = function(td, name, value)
  SetAttributeFloat(td, name, value)
end

tdft.SetAttributeString = function(td, name, value)
  SetAttributeString(td, name, value)
end

tdft.GetAttributeInt = function(td, name)
  return GetAttributeInt(td, name)
end

tdft.GetAttributeFloat = function(td, name)
  return GetAttributeFloat(td, name)
end

tdft.GetAttributeString = function(td, name)
  return GetAttributeString(td, name)
end

tdft.AddTag = function(td, tag)
  AddTag(td, tag)
end

tdft.RemoveTag = function(td, tag)
  RemoveTag(td, tag)
end

tdft.HasTags = function(td, tagStr)
  return TileHasTags(td, tagStr)
end

tdft.GetTags = function(td)
  return GetTags(td)
end
