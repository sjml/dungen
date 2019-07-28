-- merge everything into the global namespace
for k, v in pairs(dungen) do
  if (nil == _G[k]) then -- keep from overriding any built-ins
    _G[k] = v
  end
end
dungen = nil

package.path = './scripts/?.lua;' .. package.path
require("sys.lib")
require("sys.styler")
require("sys.constraint-solver")
require("sys.hlvm")

ProFi = require("lib.ProFi")
ordered_table = require("lib.ordered_table")

os.exit = function()
  io.stderr:write("Cannot exit from Lua.\n")
end

loadFiles("scripts/simulation/elements", "")

