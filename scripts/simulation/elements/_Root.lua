-- push("TestNode")


ts = TileSet()
AddTileToSet(ts, GetTileAtIndex(0))
AddTileToSet(ts, GetTileAtIndex(1))
AddTileToSet(ts, GetTileAtPosition(1, 1))
AddTileToSet(ts, GetTileAtPosition(3, 2))
o = CreateOutline(ts)
AddOutline(o)
DestroyOutline(o)

collectgarbage()
