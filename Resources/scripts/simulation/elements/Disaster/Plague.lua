caverns = GetRegionsTagged("natural, chamber")

candidates = {}
while (#candidates == 0) do
  for _, c in pairs(caverns) do
    if c:HasTags("FATE") ~= true and c:HasTags("Plague") ~= true then
      table.insert(candidates, c)
    end
  end

  if #candidates == 0 then
    sir("NaturalCavernCreationCount", 3)
    push("Primordial.NaturalCaverns")
    caverns = GetRegionsTagged("natural, chamber")
  end
end

for _, c in pairs(candidates) do
  c:AddTag("plague")
  c:SetAttributeInt("plague", c:GetAttributeInt("plague") + 1)
end
