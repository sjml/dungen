AddChoice("Watch Sim")
AddChoice("Make Choices")

PresentChoiceSelection("How do you want to play?")
sir("WaitForUI", 1)
push()
ClearChoices()

if gir("ChoiceSelection") == 1 then
  sir("InterceptDieRolls", 1)
end
