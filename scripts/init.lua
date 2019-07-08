
InitializeWorld(61, 53, 0.25)


td = GetTileAtPosition(25, 25)
print(td.i)
print(td.color)
td.color[1] = 0.0
td.color[2] = 1.0
td.color[3] = 0.0
