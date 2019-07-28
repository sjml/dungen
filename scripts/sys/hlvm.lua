
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
