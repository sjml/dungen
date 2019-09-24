
local sims = {}
local simStack = {}

local forbidden = {
  _G = 1,
  rawequal = 1,
  rawget = 1,
  rawset = 1,
  rawlen = 1,
  setfenv = 1,
  setmetatable = 1,
  getmetatable = 1,
  coroutine = 1,
  module = 1,
  require = 1,
  package = 1,
  io = 1,
  os = 1,
  debug = 1,
  loadfile = 1,
  load = 1,
  dofile = 1,

  lfs = 1,

  ResolveStyles = 1,
}

local function makeSimEnv()
  local env = {}
  for k, v in pairs(_G) do
    if (forbidden[k] == nil) then
      env[k] = _G[k]
    end
  end
  return env
end

function loadFiles(dir, prefix)
  for file in lfs.dir(dir) do
    if file ~= "." and file ~= ".." then
      if string.find(file, ".lua$") then
        local simName = file:sub(1, -5)
        local f, err = loadfile(dir .. "/" .. file)
        if (f ~= nil) then
          sims[prefix .. simName] = f
        else
          io.stderr:write("LUA ERROR: Couldn't load code for " .. prefix .. simName .. ":" .. err .. "\n")
        end
      end
      local attr = lfs.attributes(dir .. "/" .. file)
      if attr.mode == "directory" then
        loadFiles(dir .. "/" .. file, prefix .. file .. ".")
      end
    end
  end

  if (sims["Null"] == nil) then
    sims["Null"] = function() end
  end
end

function push(simName)
  if (simName == nil) then
    simName = "Null"
  end

  local f = sims[simName]
  if (f == nil) then
    io.stderr:write("LUA WARNING: No element called `" .. tostring(simName) .. "`.\n")
    f = sims["Null"]
  end

  debug.setupvalue(f, 1, makeSimEnv())
  local co = coroutine.create(f)

  table.insert(simStack, co)

  local f, r = coroutine.running()
  if not r then
    coroutine.yield()
  end
end

function HLVMProcess()
  local intitialDepth = #simStack
  if intitialDepth == 0 then return end

  local success, error = coroutine.resume(simStack[#simStack])
  if not success then
    io.stderr:write("LUA ERROR: " .. error .. "; HLVM popping.\n")
    io.stderr:write("\t" .. debug.traceback(simStack[#simStack]) .. "\n")
    table.remove(simStack)
    return
  end

  if coroutine.status(simStack[#simStack]) == "dead" then
    table.remove(simStack)
  end

  if #simStack ~= intitialDepth then
    ResolveStyles()
  end
end
