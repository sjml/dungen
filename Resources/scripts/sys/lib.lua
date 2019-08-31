function basicAssert(item1, item2, desc)
  if (desc == nil) then
    desc = ""
  else
    desc = "("..desc..")"
  end
  if (item1 == item2) then
    print("✅ OK " .. desc)
    return 0
  elseif (type(item1) ~= "table" and type(item2) == "table") then
    for _, v in pairs(item2) do
      if (item1 == v) then
        print("✅ OK " .. desc)
        return 0
      end
    end
  end
  io.stderr:write("❌ FAIL! " .. desc .. "\n")
  io.stderr:write("\t" .. tostring(item1) .. " not equal to " .. tostring(item2) .. "\n")
  return 1
end

function reportErrors(errCount)
  if errCount == 0 then
    print("\n🎉  No errors detected!")
  else
    io.stderr:write("\n🙈  " .. tostring(errCount) .. " ERROR(S)\n")
  end
end

function string:split(sep)
  local sep, fields = sep or " ", {}
  local pattern = string.format("([^%s]+)", sep)
  self:gsub(pattern, function(c) fields[#fields+1] = c end)
  return fields
end

function string:starts(startString)
  return string.sub(self, 1, string.len(startString)) == startString
end

function string:ends(endString)
  return endString == "" or string.sub(self, -string.len(endString)) == endString
end

function string:trim()
  local n = self:find"%S"
  return n and self:match(".*%S", n) or ""
end

function countInString(s1, s2)
  return select(2, s1:gsub(s2, ""))
end


function table.slice(tbl, first, last, step)
  local sliced = {}

  for i = first or 1, last or #tbl, step or 1 do
    sliced[#sliced+1] = tbl[i]
  end

  return sliced
end

function GetFunctionTable(classname)
  local reg = debug.getregistry()
  local classTable = reg["SWIG"][classname]
  if (classTable == nil) then
    io.stderr:write("LUA WARNING: No class called " .. classname .. ".\n")
    return
  end
  return classTable[".fn"]
end

