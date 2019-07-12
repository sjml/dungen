package.path = './scripts/?.lua;' .. package.path
require("sys.lib")


loadFiles("scripts/simulation/elements")
push("_Root")

