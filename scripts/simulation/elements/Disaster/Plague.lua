caverns = GetRegionsTagged("natural, chamber")

candidates = {}
for _, c in pairs(caverns) do
  if c:HasTags("FATE") ~= true and c:HasTags("Plague") ~= true then
    table.insert(candidates, c)
  end
end

for _, c in pairs(candidates) do
  c:AddTag("plague")
  c:SetAttributeInt("plague", c:GetAttributeInt("plague") + 1)
end
