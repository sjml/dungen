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
  ClearTextStrings()

  if (styles == nil) then
    return
  end

  local dims = GetWorldDimensions()
  local max = dims.x * dims.y
  for i=0, max - 1, 1 do
    local t = GetTileAtIndex(i)

    local match = {0, 0}

    for label, style in pairs(styles) do
      local m = CheckStyle(style, t)
      if  ((m[1] > match[1])  -- more specific tile match
        or (m[1] == match[1] and m[2] <= match[2])  -- equal tile match and equal or lesser region
                                                    -- match (equality loses because of ordered
                                                    -- application)
        or (match[1] <= m[1] and m[2] >= match[2])  -- tile less/equal but region equal or more
      ) then
        match = m
        ApplyFillStyle(style, t)
        ApplyLabelStyle(style, t)
      end
    end
  end

  local tilesSet = GetRenderingTileSets()
  for i=1, #tilesSet, 1 do
    local ts = tilesSet[i]

    local match = {0, 0}

    for label, style in pairs(styles) do
      local m = CheckStyle(style, ts)
      if ((m[1] > match[1]) or (match[1] <= m[1] and m[2] > match[2])) then
        match = m
        ApplyOutlineStyle(style, ts)
        -- ApplyLabelStyle(style, ts)
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
    if (target.memberSets ~= nil) then
      for _, set in pairs(target.memberSets) do
        if (set:HasTags(reqs.tags) == true) then
          match[2] = match[2] + tcount
        end
      end
    end
    if (target:HasTags(reqs.tags) == true) then
      match[1] = match[1] + tcount
    end
  end

  return match
end

function ApplyFillStyle(styleTable, target)
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

function ApplyOutlineStyle(styleTable, target)
  if (styleTable.outlineColor ~= nil) then
    local c = nil
    if (
          #styleTable.outlineColor > 2
      and type(styleTable.outlineColor[1]) == "number"
      and type(styleTable.outlineColor[2]) == "number"
      and type(styleTable.outlineColor[3]) == "number"
    ) then
      c = styleTable.outlineColor
    elseif (type(styleTable.outlineColor[1]) == "function") then
      local args = table.slice(styleTable.outlineColor, 2)
      table.insert(args, target)
      c = styleTable.outlineColor[1](table.unpack(args))
    end
    if (c ~= nil) then
      target:SetOutline(c)
    end
  end
end

function ApplyLabelStyle(styleTable, target)
  if (styleTable.labelText == nil) then
    return
  end
  local text = styleTable.labelText or ""
  local position = WorldToScreen(target.worldPos)
  -- local scale = styleTable.labelScale or 1.0
  local scale = 1.0 -- scaling messes up coords for now
  local color = styleTable.labelColor or {1.0, 1.0, 1.0}
  AddTextString(text, {position.x, position.y}, scale, color)
end
