
lazyBasicAssert(nil, AValue, "a nil before push")
lazyBasicAssert(nil, BValue, "b nil before push")
push("_testing.stackB")
lazyBasicAssert(nil, AValue, "a nil after push")
lazyBasicAssert(nil, BValue, "b nil after push")

sir("testStackB", 1)
push("_testing.stackB")
