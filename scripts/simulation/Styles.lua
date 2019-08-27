local colors = { -- R      G      B
  RED =          { 1.0 ,  0.0 ,  0.0  },
  YELLOW =       { 1.0 ,  1.0 ,  0.0  },
  MAGENTA =      { 1.0 ,  1.0 ,  0.0  },
  CYAN =         { 0.0 ,  1.0 ,  1.0  },
  BLACK =        { 0.0 ,  0.0 ,  0.0  },
  WHITE =        { 1.0 ,  1.0 ,  1.0  },

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

sky = {
  reqs = {
    tags = "sky"
  },
  tileFill = colors.sky
}

groundBase = {
  reqs = {
    tags = "ground"
  },
  tileFill = {colorTable, "GroundColors", "depth"}
}

openCave = {
  reqs = {
    tags = "ground",
    attributes = {
      {"open", Equal, 1}
    }
  },
  tileFill = colors.openCave
}

mithralUP = {
  reqs = {
    tags = "mithralUP"
  },
  labelText = "Mithral",
  labelSize = 12.0,
  labelOffset = {0, -0.3},
}

mithralDOWN = {
  reqs = {
    tags = "mithralDOWN"
  },
  labelText = "Mithral",
  labelSize = 12.0,
  labelOffset = {0, 0.3},
}

mithral = {
  reqs = {
    tags = "mithral"
  },
  tileFill = colors.mithral,
}

water = {
  reqs = {
    tags = "ground, water"
  },
  tileFill = colors.water,
  labelText = "Water",
}

magma = {
  reqs = {
    tags = "ground, magma"
  },
  tileFill = colors.magma,
  labelText = "Magma",
}

plague = {
  reqs = {
    tags = "ground, plague"
  },
  tileFill = colors.plague,
  labelText = "Plague",
}

gems = {
  reqs = {
    tags = "ground, gems"
  },
  tileFill = colors.gems,
  labelText = "Gems!",
}

monster = {
  reqs = {
    tags = "ground, monster"
  },
  tileFill = colors.monster,
}

FATE = {
  reqs = {
    tags = "ground, FATE"
  },
  tileFill = colors.FATE,
  labelText = "FATE",
}
