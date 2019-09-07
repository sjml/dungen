local cavernCenter = GetTileAtPosition(5, 20)
local chamber = makeRoundChamber(cavernCenter, 1)
chamber:AddTag("natural")

-- local extension = {
--   GetTileAtPosition(4, 25),
--   GetTileAtPosition(5, 25),
--   GetTileAtPosition(6, 25),
-- }
-- for _, t in pairs(extension) do
--   AddTileToRegion(chamber, t)
--   t:SetAttributeInt("open", 1)
-- end

-- local out = GetTileAtPosition(7, 23)
-- out:SetAttributeInt("open", 1)
-- out = GetTileAtPosition(7, 25)
-- out:SetAttributeInt("open", 1)
-- out = GetTileAtPosition(4, 26)
-- out:SetAttributeInt("open", 1)
