AddBanner("Primordial Age", 2.5, {0, 0, 0}, {.5, .5, .5, .6}, 2.0)
sir("WaitForUI", 1)
push()

numRolls = 3
if gir("InterceptDieRolls") == 1 then
  numRolls = 6
end
for i=1,numRolls do
  push("Primordial._DieRoll")
end
