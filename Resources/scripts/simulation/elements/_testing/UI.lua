AddBanner("Choice Coming Up...", 3.0, {1.0, 1.0, 1.0, 1.0}, {0.2, 0.2, 0.2, 0.5}, 2)
sir("WaitForUI", 1)
push()

AddChoice("Option A")
AddChoice("Option B")
AddChoice("Option C")
AddChoice("Option D")
AddChoice("Option E")
AddChoice("Option F")
AddChoice("Option G")
AddChoice("Option H")
-- AddChoice("Option I")
-- AddChoice("Option J")
-- AddChoice("Option K")
-- AddChoice("Option L")
PresentChoiceSelection("Multiple Choice Test")
sir("WaitForUI", 1)
push()
ClearChoices()

valids = TileSet()
dims = GetWorldDimensions()
for x = dims.x // 2, dims.x - 1 do
  for y = dims.y // 2, dims.y - 1 do
    valids = AddTileToSet(valids, GetTileAtPosition(x, y))
  end
end

stsr("TileChoiceValidSet", valids)
AddBanner("Tile Choice Coming Up...", 2.5, {1.0, 1.0, 1.0, 1.0}, {0.2, 0.2, 0.2, 0.5}, 2)
sir("WaitForUI", 1)
push()

PresentTileChoice()
sir("WaitForUI", 1)
push()

DestroyTileSet(valids)
