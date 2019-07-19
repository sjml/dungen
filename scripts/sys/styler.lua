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
  styles = {}
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

    for label, style in pairs(styles) do
      if (CheckStyle(style, t)) then
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

  if (reqs.tags ~= nil) then
    if (target:HasTags(reqs.tags) ~= true) then
      return false
    end
  end

  return true
end

function ApplyStyle(styleTable, target)
  if (styleTable.fill ~= nil) then
    if (
          #styleTable.fill > 2
      and type(styleTable.fill[1]) == "number"
      and type(styleTable.fill[2]) == "number"
      and type(styleTable.fill[3]) == "number"
    ) then
      target.color = styleTable.fill
    elseif (type(styleTable.fill[1]) == "function") then
      local args = table.slice(styleTable.fill, 2)
      table.insert(args, target)
      target.color = styleTable.fill[1](table.unpack(args))
    end
  end
end
