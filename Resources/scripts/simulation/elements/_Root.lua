-- push("_testing._Root")
-- push("_testing.UnitTest")
-- push("_testing.UI")

-- push("Meta.Splash")
-- push("Meta.Intro")

-- push("Primordial._Root")

-- push("Meta.Outro")

push("Primordial.GoldVein")

local cavernCenter = GetTileAtPosition(30, 21)
local chamber = makeChamber(cavernCenter, 1)
chamber:AddTag("natural")

-- sir("DieSides", 4)
-- push("System.DieRoll")
-- for i=1,gir("DieRollResult") do
--   storeInChamber(chamber, "gems", true)
-- end
-- chamber:SetAttributeInt("treasure", gir("DieRollResult") + chamber:GetAttributeInt("treasure"))

chamber:AddTag("plague")

push("Primordial.FlowingRiver")
push("Disaster.VolcanicEruption")
