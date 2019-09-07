-- push("_testing._Root")
-- push("_testing.UnitTest")
-- push("_testing.UI")
-- push("_testing.Pathing")

-- push("Meta.Splash")
-- push("Meta.Intro")

-- push("Primordial._Root")

-- push("Meta.Outro")

-- push("Primordial.GoldVein")

-- push("_testing.WaterFlowPlayground")

local cavernCenter = GetTileAtPosition(5, 20)
local chamber = makeRoundChamber(cavernCenter, 1)
chamber:AddTag("natural")
chamber:AddTag("plague")

-- push("Primordial.FlowingRiver")
-- SeedRandomString("140734188874162///4364600400") -- chamber forming around gold vein, not washing
SeedRandomString("140734188878915///4375671888") -- chambers need to be able to merge
push("Primordial.FlowingRiverV2")
-- push("Disaster.VolcanicEruption")
