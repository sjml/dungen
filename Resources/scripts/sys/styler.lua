-- Allows for a more declarative approach the visual appearance of the hex world.
--
local Set = require("sys.set")

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
    colorTable = colorTable,
    LessThan = LessThan,
    LessThanOrEqual = LessThanOrEqual,
    GreaterThan = GreaterThan,
    GreaterThanOrEqual = GreaterThanOrEqual,
    Equal = Equal,
    NotEqual = NotEqual,
  }
  styles = ordered_table()
  for k,v in pairs(preload) do styles[k] = v end
  local styleLoad, err = loadfile("scripts/simulation/Styles.lua", "bt", styles)
  if (err) then
    io.stderr:write("ERROR: Could not load styles. " .. err .. "\n")
    return
  end
  styleLoad()
  for k,v in pairs(preload) do
    styles[k] = nil
  end

  ResolveStyles()
end

-- returns true if m1 should have priority over m2
--   (equality loses because of ordered application)
local function higherMatch(m1, m2)
  if  (
          (m1[1] + m1[2] >= m2[1] + m2[2])  -- sum of matches is equal
    or (m1[1] > m2[1])                      -- more specific tile match
    or (m1[1] == m2[1] and m1[2] <= m2[2])  -- equal tile match and equal or lesser region
                                            -- match
    or (m2[1] <= m1[1] and m1[2] >= m2[2])  -- tile less/equal but region equal or more
  )
  then
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
    local tags = Set:new(reqs.tags:split(", "))
    if target.meta ~= nil and target.meta.memberRegions ~= nil then
      for _, reg in pairs(target.meta.memberRegions) do
        local rTags = reg:GetTags()
        for _, rt in pairs(rTags) do
          if tags[rt] ~= nil then
            tags[rt] = nil
            match[2] = match[2] + 1
          end
        end
      end
    end
    for _, tt in pairs(target:GetTags()) do
      if tags[tt] ~= nil then
        tags[tt] = nil
        match[1] = match[1] + 1
      end
    end

    if #tags:toList() > 0 then
      return {-1, -1}
    end
  end

  if (reqs.attributes ~= nil) then
    for _, attrCheck in pairs(reqs.attributes) do
      if (target:CheckAttribute(attrCheck[1], attrCheck[2], attrCheck[3])) then
        match[1] = match[1] + 1
      else
        match[1] = 0
      end
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
      target.draw.color = styleTable.tileFill
    elseif (type(styleTable.tileFill[1]) == "function") then
      local args = table.slice(styleTable.tileFill, 2)
      table.insert(args, target)
      target.draw.color = styleTable.tileFill[1](table.unpack(args))
    end
  end
end

local function applyOutlineStyle(styleTable, target)
  if (styleTable.outlineColor == nil) then
    return
  end
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
    local outlineType = styleTable.outlineType or "center"
    if outlineType == "center" then
      outlineType = 0
    elseif outlineType == "inner" then
      outlineType = 1
    elseif outlineType == "outer" then
      outlineType = -1
    else
      io.stderr:write("STYLE ERROR: Unknown outlineType: " .. tostring(outlineType) .. "\n")
      return
    end
    local thickness = styleTable.outlineThickness or 0.15
    target:SetOutline(c, thickness, outlineType)
  end
end

local function applyLabelStyle(styleTable, target)
  if (styleTable.labelText == nil) then
    return
  end
  local text = styleTable.labelText or ""
  local scale = styleTable.labelSize or 16.0
  local color = styleTable.labelColor or {0.0, 0.0, 0.0}
  local offset = styleTable.labelOffset or {0, 0}

  SetRegionLabel(target, text, scale, color, offset)
end


function ResolveStyles()
  if (styles == nil) then
    return
  end

  local dirtyTiles = GetDirtyTiles()
  for _, t in pairs(dirtyTiles) do
    ClearTileOutline(t)

    local match = {0, 0}

    for label, style in pairs(styles) do
      local m = checkStyle(style, t)
      if higherMatch(m, match) then
        match = m
        applyOutlineStyle(style, t)
        applyFillStyle(style, t)
      end
    end
  end
  CleanAllTiles()

  local dirtyRegions = GetDirtyRegions()
  for _, reg in pairs(dirtyRegions) do
    ClearRegionLabel(reg)
    ClearRegionOutline(reg)

    local match = {0, 0}

    for label, style in pairs(styles) do
      local m = checkStyle(style, reg)
      if higherMatch(m, match) then
        match = m
        applyOutlineStyle(style, reg)
        applyLabelStyle(style, reg)
      end
    end
  end
  CleanAllRegions()
end
