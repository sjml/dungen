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

local function higherMatch(m1, m2)
  if  ((m1[1] > m2[1])  -- more specific tile match
    or (m1[1] == m2[1] and m1[2] <= m2[2])  -- equal tile match and equal or lesser region
                                            -- match (equality loses because of ordered
                                            -- application)
    or (m2[1] <= m1[1] and m1[2] >= m2[2])  -- tile less/equal but region equal or more
  ) then
    return true
  else
    return false
  end
end

local function checkStyle(styleTable, target)
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

local function applyFillStyle(styleTable, target)
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

local function applyOutlineStyle(styleTable, target)
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
      thickness = styleTable.outlineThickness or 0.15
      target:SetOutline(c, thickness)
    end
  end
end

local function applyLabelStyle(styleTable, target)
  if (styleTable.labelText == nil) then
    return
  end
  local text = styleTable.labelText or ""
  local scale = styleTable.labelSize or 14.0
  local color = styleTable.labelColor or {1.0, 1.0, 1.0}

  local sPos = WorldToScreen(target.worldPos)
  local extents = MeasureTextExtents(text, "fonts/04B_03__.TTF", scale)
  sPos.x = sPos.x - (extents.x / 2)
  sPos.y = sPos.y + (extents.y / 2)

  AddTileLabel(text, {sPos.x, sPos.y}, scale, color)
end


function ResolveStyles()
  ClearTileLabels()

  if (styles == nil) then
    return
  end

  local dims = GetWorldDimensions()
  local max = dims.x * dims.y
  for i=0, max - 1, 1 do
    local t = GetTileAtIndex(i)

    local match = {0, 0}

    for label, style in pairs(styles) do
      local m = checkStyle(style, t)
      if higherMatch(m, match) then
        match = m
        applyFillStyle(style, t)
        applyLabelStyle(style, t)
      end
    end
  end

  local tilesSet = GetRenderingTileSets()
  for i=1, #tilesSet, 1 do
    local ts = tilesSet[i]

    local match = {0, 0}

    for label, style in pairs(styles) do
      local m = checkStyle(style, ts)
      if higherMatch(m, match) then
        match = m
        applyOutlineStyle(style, ts)
        -- applyLabelStyle(style, ts)
      end
    end
  end
end
