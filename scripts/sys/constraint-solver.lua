local class = require("lib.middleclass")
local Set = require("sys.set")
local TileDataSet = require("sys.tiledataset")

--------------- Base Constraint
Constraint = class("Constraint")



--------------- Set Constraint Type
SetConstraint = class("SetConstraint", Constraint)
function SetConstraint:initialize()
  Constraint:initialize(self)
end

function SetConstraint:GetTiles()
  return TileDataSet:new()
end

-- A naive approach that kicks in when a derived class
--   doesn't have a means of filtering on its own.
function SetConstraint:FilterTiles(tiles)
  local ts = self:GetTiles()
  return ts:intersection(tiles)
end



--------------- Check Constraint Type
CheckConstraint = class("CheckConstraint", Constraint)
function CheckConstraint:initialize()
  Constraint:initialize(self)
end

function CheckConstraint:CheckTile(t)
  return false
end



--------------- Has All Tags
HasAllTags = class("HasAllTags", SetConstraint)
function HasAllTags:initialize(tagStr)
  SetConstraint:initialize(self)
  self.tagStr = tagStr
end

function HasAllTags:GetTiles()
  local tagged = GetTilesTagged(self.tagStr)
  return TileDataSet:new(tagged)
end



--------------- Has None of Tags
HasNoneOfTags = class("HasNoneOfTags", CheckConstraint)
function HasNoneOfTags:initialize(tagStr)
  CheckConstraint:initialize(self)
  self.tags = {}
  local ts = tagStr:split(",")
  for _, t in ipairs(ts) do
    t = t:trim()
    table.insert(self.tags, t)
  end
  self.tags = Set:new(self.tags)
end

function HasNoneOfTags:CheckTile(t)
  for _, tag in ipairs(t:GetTags()) do
    if self.tags[tag] ~= nil then return false end
  end
  return true
end



--------------- In Tile Range
InTileRange = class("InTileRange", SetConstraint)
function InTileRange:initialize(topLeft, bottomRight)
  self.topLeft = topLeft
  self.bottomRight = bottomRight
end

function InTileRange:GetTiles()
  local ret = {}
  for i = self.topLeft[1], self.bottomRight[1] do
    for j = self.topLeft[2], self.bottomRight[2] do
      table.insert(ret, GetTileAtPosition(i, j))
    end
  end
  return TileDataSet:new(ret)
end



--------------- Attribute Set
HasAttributes = class("HasAttributes", SetConstraint)
function HasAttributes:initialize(attrName, comp, value)
  self.attrName = attrName
  self.comp = comp
  self.value = value
end

function HasAttributes:GetTiles()
  local ts = GetTilesByAttribute(self.attrName, self.comp, tostring(self.value))
  return TileDataSet:new(ts)
end






--------------- the actual solver
ConstraintSolver = class("ConstraintSolver")
function ConstraintSolver:initialize(constraints)
  self.constraints = constraints
  self.solved = false
  self.pickedTileSet = nil
  self.pickedTile = nil
end

function ConstraintSolver:Solve()
  local tiles = TileDataSet:new(GetAllTiles())
  for i, c in ipairs(self.constraints) do
    -- print("before iteration " .. tostring(i) .. " | " .. tostring(#tiles:toList()) .. " tiles.")
    if c:isInstanceOf(SetConstraint) then
      tiles = c:FilterTiles(tiles)
    elseif c:isInstanceOf(CheckConstraint) then
      for t, _ in pairs(tiles) do
        if not c:CheckTile(t) then
          tiles[t] = nil
        end
      end
    end
    -- print("after iteration " .. tostring(i) .. " | " .. tostring(#tiles:toList()) .. " tiles.")
  end

  local tList = tiles:toList()
  self.pickedTileSet = TileSet()
  for _, t in ipairs(tList) do
    self.pickedTileSet:AddTile(t)
  end
  if #tList > 0 then
    self.pickedTile = tList[RandomRangeInt(1, #tList)]
  end

  return true
end
