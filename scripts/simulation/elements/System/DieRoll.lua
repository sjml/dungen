
if gir("InterceptDieRolls") == 1 and gir("DieRollInterceptable") == 1 then
  local lastChoice = ""
  local choiceMappings = {}
  local choiceCount = 0
  for i=1,gir("DieSides") do
    local choice = gsr("DieRollChoice" .. tostring(i))
    if choice ~= lastChoice then
      lastChoice = choice
      AddChoice(choice)
      choiceMappings[choiceCount] = i
      choiceCount = choiceCount + 1
    end
  end

  PresentChoiceSelection()
  sir("WaitForUI", 1)
  push()
  ClearChoices()

  sir("DieRollResult", choiceMappings[gir("ChoiceSelection")])
else
  sir("DieRollResult", RandomRangeInt(1, gir("DieSides")))
end
