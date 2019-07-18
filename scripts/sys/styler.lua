-- Allows for a more declarative approach the visual appearance of the hex world.
--

local styles = nil

function ReloadStyles()
  styles = {
    LoadColorTable = LoadColorTable
  }
  styleLoad, err = loadfile("scripts/simulation/Styles.lua", "bt", styles)
  if (err) then
    io.stderr:write("ERROR: Could not load styles. " .. err .. "\n")
    return
  end
  styleLoad()
  styles.LoadColorTable = nil

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
    if (type(styleTable.fill[1]) == "number") then
      target.color = styleTable.fill
    elseif (styleTable.fill[1] == "ColorTable") then
      target.color = GetColorByPercent(
        styleTable.fill[2],
        target:GetAttributeFloat("depth")
      )
    end
  end
end
