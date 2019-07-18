
local TileSet = {
  Width  = 61,
  Height = 53,
  Scale  = 0.25,
}

InitializeWorld(TileSet.Width, TileSet.Height, TileSet.Scale)

local GroundLine = 5;
for j=0, TileSet.Height - 1 do
  local percent = (j - 1 - GroundLine) / (TileSet.Height - GroundLine)

  for i=0, TileSet.Width - 1 do
    local t = GetTileAtPosition(i, j)
    if (j < GroundLine) then
      t:AddTag("sky")
    else
      t:AddTag("ground")
      t:SetAttributeFloat("depth", percent)
    end
  end
end

ReloadStyles()
