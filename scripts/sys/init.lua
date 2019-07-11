
local sims = {}

function loadFiles(dir)
  for file in lfs.dir(dir) do
    if string.find(file, ".lua$") then
      local simName = file:sub(1, -5)
      local f, err = loadfile(dir .. "/" .. file)
      if (f ~= nil) then
        sims[simName] = f
      else
        print("ERROR: Couldn't load code for " .. simName .. ":")
        print(err)
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
    print("WARNING: No element called " .. sim .. ".")
    f = sims["Null"]
  end

  return coroutine.wrap(f)
end

function push(className)
  if (className == nil) then
    className = "Null"
  end

  sim = dungen.CreateSimulationElement(className)
  dungen.HLVMPush(sim)

  local f, r = coroutine.running()
  if (r ~= true) then
    coroutine.yield(1)
  end
end


loadFiles("scripts/simulation/elements")

push("_Root")

