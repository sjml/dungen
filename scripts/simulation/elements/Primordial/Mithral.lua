local padding = 2

local mithralConstraints = {
  InTileRange(
    {padding, gir("GroundLine") + padding},
    {gir("TileWidth") - padding, gir("TileHeight") - padding}
  ),
  HasAllTags("ground"),
  HasAttributes("open", Equal, 0),
}
local mithralSolver = ConstraintSolver(mithralConstraints)
mithralSolver:Solve()

local base = mithralSolver.pickedTile
base:AddTag("mithral")

if (base.hexPos.y > (gir("GroundHeight") / 2)) then
  -- bottom part of the field; face down
  GetTileAtIndex(base.neighborNE):AddTag("mithral")
  GetTileAtIndex(base.neighborNW):AddTag("mithral")
else
  -- top part of the field; face up
  GetTileAtIndex(base.neighborSE):AddTag("mithral")
  GetTileAtIndex(base.neighborSW):AddTag("mithral")
end
