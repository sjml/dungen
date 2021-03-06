padding = 2

mithralConstraints = {
  InTileRange(
    {padding, gir("GroundLine") + padding},
    {gir("TileWidth") - padding, gir("TileHeight") - padding}
  ),
  HasAllTags("ground"),
  HasAttributes("open", Equal, 0),
}
mithralSolver = ConstraintSolver(mithralConstraints)
mithralSolver:Solve()

base = mithralSolver.pickedTile
base:AddTag("mithral")

reg = CreateRegion()
reg:AddTile(base)

local t1 = nil
local t2 = nil
if (base.hexPos.y > (gir("GroundHeight") / 2)) then
  -- bottom part of the field; face down
  reg:AddTag("mithralDOWN")
  t1 = GetTileAtIndex(base.neighborNE)
  t2 = GetTileAtIndex(base.neighborNW)
else
  -- top part of the field; face up
  reg:AddTag("mithralUP")
  t1 = GetTileAtIndex(base.neighborSE)
  t2 = GetTileAtIndex(base.neighborSW)
end

t1:AddTag("mithral")
t2:AddTag("mithral")
reg:AddTile(t1)
reg:AddTile(t2)
