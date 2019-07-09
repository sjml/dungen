
InitializeWorld(61, 53, 0.25)


td = GetTileAtPosition(25, 25)
print(td.i)
print(td.color)
td.color[1] = 0.0
td.color[2] = 1.0
td.color[3] = 0.0

td2 = GetTileAtPosition(26, 25)
td2.color = {0, 1, 0}
