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
basicAssert(#getItemsInChamber(c), 7, "Storing items in a chamber")

removeItemFromChamber(c, "gold")
removeItemFromChamber(c, "gold")
removeItemFromChamber(c, "gold")
removeItemFromChamber(c, "water")
check(c)
basicAssert(#getItemsInChamber(c), 3, "Removing items in a chamber")


base2 = GetTileAtPosition(40, 40)
c2 = makeRoundChamber(base2, 1, nil)
c2:AddTag("plague")

base3 = GetTileAtPosition(41, 38)
c3 = makeRoundChamber(base3, 1, nil)
