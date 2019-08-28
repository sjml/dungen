
sir("DieSides", 8)
push("System.DieRoll")

local result = gir("DieRollResult")
if     (result >= 1 and result <= 3) then
  push("Disaster.Earthquake")
elseif (result == 4) then
  push("Disaster.VolcanicEruption")
elseif (result == 5) then
  if (gir("RiverCount") <= 2) then
    push("Primordial.UndergroundRiver")
  else
    push("Disaster.Earthquake")
  end
elseif (result == 6) then
  push("Disaster.Plague")
elseif (result == 7) then
  push("Disaster.FallenStar")
elseif (result == 8) then
  push("Primordial._DieRoll")
end
