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
local mithralPit = {}
table.insert(mithralPit, base)

if (base.hexPos.y > (gir("GroundHeight") / 2)) then
  -- bottom part of the field; face down
  table.insert(mithralPit, GetTileAtIndex(base.neighborNE))
  table.insert(mithralPit, GetTileAtIndex(base.neighborNW))
else
  -- top part of the field; face up
  table.insert(mithralPit, GetTileAtIndex(base.neighborSE))
  table.insert(mithralPit, GetTileAtIndex(base.neighborSW))
end

for _, m in pairs(mithralPit) do
  m:AddTag("mithral")
end
