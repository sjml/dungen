
local TileSet = {
  Width  = 61,
  Height = 53,
  Scale  = 0.25,
}

InitializeWorld(TileSet.Width, TileSet.Height, TileSet.Scale)

local GroundLine = 3;
for j=0, TileSet.Height - 1 do
  local percent = (j - 1 - GroundLine) / (TileSet.Height - GroundLine)

  for i=0, TileSet.Width - 1 do
    local t = GetTileAtPosition(i, j)
    if (j < GroundLine) then
      t:AddTag("sky")
      t:SetAttributeInt("open", 1)
    else
      t:AddTag("ground")
      t:SetAttributeFloat("depth", percent)
      t:SetAttributeInt("open", 0)
    end
  end
end

sir("GroundLine", GroundLine)
sir("GroundHeight", TileSet.Height - GroundLine)
sir("TileWidth", TileSet.Width)
sir("TileHeight", TileSet.Height)
sir("RandomTileXMin", 0)
sir("RandomTileXMax", TileSet.Width - 1)
sir("RandomTileYMin", GroundLine)
sir("RandomTileYMax", TileSet.Height - 1)

LoadColorTable("GroundColors", "images/DunStone.png")

ReloadStyles()
