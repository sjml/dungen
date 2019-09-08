function check(c)
  local tags = getItemsInChamber(c)
  print(tostring(#tags) .. " items:")
  for i, t in ipairs(tags) do
    print("  "..tostring(i)..": "..t)
  end
  print()
end

base = GetTileAtPosition(25, 25)
c = makeRoundChamber(base, 1, nil)

storeItemInChamber(c, "gold")
storeItemInChamber(c, "gold")
storeItemInChamber(c, "gold")
storeItemInChamber(c, "water")
storeItemInChamber(c, "gold")
storeItemInChamber(c, "monster")
storeItemInChamber(c, "gold")
check(c)

removeItemFromChamber(c, "gold")
removeItemFromChamber(c, "gold")
removeItemFromChamber(c, "gold")
removeItemFromChamber(c, "water")
check(c)
