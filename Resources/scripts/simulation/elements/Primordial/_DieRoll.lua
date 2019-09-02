
sir("DieRollInterceptable", 1)
ssr("DieRollDescription", "Primordial Age Event")

ssr("DieRollChoice1", "Mithral")
ssr("DieRollChoice2", "Caverns")
ssr("DieRollChoice3", "Caverns")
ssr("DieRollChoice4", "Gold Vein")
ssr("DieRollChoice5", "Cave Complex")
ssr("DieRollChoice6", "River")
ssr("DieRollChoice7", "Great Wyrm")
ssr("DieRollChoice8", "Disaster")

sir("DieSides", 8)
push("System.DieRoll")
sir("DieRollInterceptable", 0)

result = gir("DieRollResult")
if     (result == 1) then
  push("Primordial.Mithral")
  push("Primordial.Mithral")
elseif (result == 2 or result == 3) then
  sir("NaturalCavernCreationCount", 6)
  push("Primordial.NaturalCaverns")
elseif (result == 4) then
  push("Primordial.GoldVein")
elseif (result == 5) then
  push("Primordial.CaveComplex")
elseif (result == 6) then
  push("Primordial.UndergroundRiver")
elseif (result == 7) then
  push("Primordial.GreatWyrm")
elseif (result == 8) then
  push("Disaster._DieRoll")
end

if gir("InterceptDieRolls") == 1 then
  sfr("WaitDuration", 2.0)
  push("System.Wait")
end
