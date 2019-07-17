
local TileSet = {
  Width  = 61,
  Height = 53,
  Scale  = 0.25,
}

local GroundLine = 5;

local colors = { -- R      G      B
  openCave =     { 0.7 ,  0.7 ,  0.7  },
  sky =          { 0.38,  0.82,  1.0  },
  monster =      { 0.0 ,  0.35,  0.2  },
  gold =         { 1.0 ,  0.85,  0.0  },
  mithral =      { 0.0 ,  1.0 ,  0.7  },
  plague =       { 0.68,  0.81,  0.47 },
  gems =         { 0.91,  0.21,  0.76 },
  FATE =         { 1.0 ,  1.0 ,  1.0  },
  water =        { 0.0 ,  0.45,  1.0  },
  sickWater =    { 0.34,  0.63,  0.74 },
  magma =        { 1.0 ,  0.33,  0.0  },
  meteorite =    { 0.38,  0.0 ,  1.0  },
  dwarven =      { 0.0 ,  0.0 ,  1.0  },
}

InitializeWorld(TileSet.Width, TileSet.Height, TileSet.Scale)

LoadColorTable("GroundColors", "images/DunStone.png")

for j=0, TileSet.Height - 1 do
  local percent = (j - 1 - GroundLine) / (TileSet.Height - GroundLine)

  for i=0, TileSet.Width - 1 do
    local t = GetTileAtPosition(i, j)
    if (j < GroundLine) then
      t.color = colors.sky
    else
      t.color = GetColorByPercent("GroundColors", percent)
    end
  end
end
