-- Allows for a more declarative approach the visual appearance of the hex world.
--

local styles = nil

local function colorTable(name, value, tile)
  if (type(value) == "string") then
    local f = tile:GetAttributeFloat(value)
    return GetColorByPercent(name, f)
  end
  return GetColorByPercent(name, value)
end

function ReloadStyles()
  local preload = {
    colorTable = colorTable
  }
  styles = ordered_table()
  for k,v in pairs(preload) do styles[k] = v end
  styleLoad, err = loadfile("scripts/simulation/Styles.lua", "bt", styles)
  if (err) then
    io.stderr:write("ERROR: Could not load styles. " .. err .. "\n")
    return
  end
  styleLoad()
  for k,v in pairs(styles) do
    if type(v) == "function" then
      styles[k] = nil
    end
  end

  ResolveStyles()
end

function ResolveStyles()
  local dims = GetWorldDimensions()
  local max = dims.x * dims.y
  for i=0, max - 1, 1 do
    local t = GetTileAtIndex(i)

    local match = {0, 0}

    for label, style in pairs(styles) do
      local m = CheckStyle(style, t)
      if ((m[1] > match[1]) or (match[1] <= m[1] and m[2] > match[2])) then
        match = m
        ApplyStyle(style, t)
      end
    end
  end
end

function CheckStyle(styleTable, target)
  local reqs = styleTable.reqs
  if (reqs == nil) then
    return true
  end

  local match = {0, 0}

  if (reqs.tags ~= nil) then
    local tcount = countInString(reqs.tags, ",") + 1
    for _, set in pairs(target.memberSets) do
      if (set:HasTags(reqs.tags) == true) then
        match[2] = match[2] + tcount
      end
    end
    if (target:HasTags(reqs.tags) == true) then
      match[1] = match[1] + tcount
    end
  end

  return match
end

function ApplyStyle(styleTable, target)
  if (styleTable.tileFill ~= nil) then
    if (
          #styleTable.tileFill > 2
      and type(styleTable.tileFill[1]) == "number"
      and type(styleTable.tileFill[2]) == "number"
      and type(styleTable.tileFill[3]) == "number"
    ) then
      target.color = styleTable.tileFill
    elseif (type(styleTable.tileFill[1]) == "function") then
      local args = table.slice(styleTable.tileFill, 2)
      table.insert(args, target)
      target.color = styleTable.tileFill[1](table.unpack(args))
    end
  end
end
