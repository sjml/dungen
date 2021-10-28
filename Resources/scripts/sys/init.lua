-- merge everything into the global namespace
for k, v in pairs(dungen) do
  if (nil == _G[k]) then -- keep from overriding any built-ins
    _G[k] = v
  end
end
dungen = nil

package.path = './scripts/?.lua;' .. package.path
local debug = false
if (debug) then
  -- using ZeroBrane on the Mac to debug Lua occasionally
  package.path = package.path .. ";/Applications/ZeroBraneStudio.app/Contents/ZeroBraneStudio/lualibs/?/?.lua"
  package.path = package.path .. ";/Applications/ZeroBraneStudio.app/Contents/ZeroBraneStudio/lualibs/?.lua"
  package.cpath = package.cpath .. ";/Applications/ZeroBraneStudio.app/Contents/ZeroBraneStudio/bin/clibs53/?/?.dylib"
  package.cpath = package.cpath .. ";/Applications/ZeroBraneStudio.app/Contents/ZeroBraneStudio/bin/clibs53/?.dylib"

  require('mobdebug').start()
end

require("lib.strict")

require("sys.lib")
require("sys.styler")
require("sys.constraint-solver")
require("sys.hlvm")

require("sys.sugar.hlvm")
require("sys.sugar.tiledata")
require("sys.sugar.region")
require("sys.sugar.agent")

require("sys.gamelib.chamber")
require("sys.gamelib.pathing")
require("sys.gamelib.exploration")

ProFi = require("lib.ProFi")
ordered_table = require("lib.ordered_table")

function print(...)
  local args = table.pack(...)
  for i=1, #args do
    local s = args[i]
    if (s ~= nil) then
      Log(s)
    else
      Log("[nil]")
    end
    if (i < #args) then
      Log(" ")
    end
  end
  Log("\n")
end

os.exit = function()
  LogErr("Cannot exit from Lua.\n")
end

loadFiles("scripts/simulation/elements", "")

