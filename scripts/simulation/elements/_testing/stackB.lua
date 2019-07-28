
lazyBasicAssert(nil, AValue, "a nil before declaration")
lazyBasicAssert(nil, BValue, "b nil before declaration")
AValue = "A"
local BValue = "B"
lazyBasicAssert("A", AValue, "a not nil after declaration")
lazyBasicAssert("B", BValue, "b not nil after declaration")
