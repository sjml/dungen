local function lazyBasicAssert(item1, item2, desc)
  if (desc == nil) then
    desc = ""
  else
    desc = "("..desc..")"
  end
  if (item1 == item2) then
    print("✅ OK " .. desc)
    return
  elseif (type(item1) ~= "table" and type(item2) == "table") then
    for _, v in pairs(item2) do
      if (item1 == v) then
        print("✅ OK " .. desc)
        return
      end
    end
  end
  io.stderr:write("❌ FAIL! " .. desc .. "\n")
  io.stderr:write("\t" .. tostring(item1) .. " not equal to " .. tostring(item2) .. "\n")
end

lazyBasicAssert(nil, AValue, "a nil before declaration")
lazyBasicAssert(nil, BValue, "b nil before declaration")
AValue = "A"
local BValue = "B"
lazyBasicAssert("A", AValue, "a not nil after declaration")
lazyBasicAssert("B", BValue, "b not nil after declaration")
