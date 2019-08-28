
sir("DieSides", 8)
push("System.DieRoll")

result = gir("DieRollResult")
if     (result == 1) then
  push("Primordial.Mithral")
  push("Primordial.Mithral")
elseif (result == 2 or result == 3) then
  push("Primordial.NaturalCaverns")
elseif (result == 4) then
  push("Primordial.GoldVein")
elseif (result == 5) then
  push("Primordial.CaveComplex")
elseif (result == 6) then
  if (gir("RiverCount") <= 2) then
    push("Primordial.UndergroundRiver")
  else
    push("Primordial.NaturalCaverns")
  end
elseif (result == 7) then
  push("Primordial.GreatWyrm")
elseif (result == 8) then
  push("Disaster._DieRoll")
end
