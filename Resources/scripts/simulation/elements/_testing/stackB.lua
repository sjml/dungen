
lazyBasicAssert(nil, AValue, "a nil before declaration")
lazyBasicAssert(nil, BValue, "b nil before declaration")
AValue = "A"
local BValue = "B"
lazyBasicAssert("A", AValue, "a not nil after declaration")
lazyBasicAssert("B", BValue, "b not nil after declaration")

if gir("testStackB") > 0 then
  sir("testStackB", 0)
  push("_testing.stackB")
end
