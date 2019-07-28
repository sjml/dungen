
lazyBasicAssert(nil, AValue, "a nil before push")
lazyBasicAssert(nil, BValue, "b nil before push")
push("stackB")
lazyBasicAssert(nil, AValue, "a nil after push")
lazyBasicAssert(nil, BValue, "b nil after push")
