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

function SetConstraint:GetPassingTiles()
  return TileDataSet:new()
end

-- A naive approach that kicks in when a derived class
--   doesn't have a means of filtering on its own.
function SetConstraint:FilterTiles(tiles)
  local ts = self:GetPassingTiles()
  return ts:intersection(tiles)
end



--------------- Check Constraint Type
CheckConstraint = class("CheckConstraint", Constraint)
function CheckConstraint:initialize()
  Constraint:initialize(self)
end

function CheckConstraint:CheckTile(ti)
  return false
end



--------------- All Tiles
-- basically a dummy constraint that just provides all
-- the tiles as a set
AllTiles = class("AllTiles", SetConstraint)
function AllTiles:initialize()
end

function AllTiles:GetPassingTiles()
  local all = GetAllTiles()
  return TileDataSet:new(all)
end



--------------- Has All Tags
HasAllTags = class("HasAllTags", SetConstraint)
function HasAllTags:initialize(tagStr)
  SetConstraint:initialize(self)
  self.tagStr = tagStr
end

function HasAllTags:GetPassingTiles()
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

function HasNoneOfTags:CheckTile(ti)
  local td = GetTileAtIndex(ti)
  for _, tag in ipairs(td:GetTags()) do
    if self.tags[tag] ~= nil then return false end
  end
  return true
end



--------------- Min Distance From Attribute
MinDistanceFromAttribute = class("MinDistanceFromAttribute", CheckConstraint)
function MinDistanceFromAttribute:initialize(tileDistance, attrName, comp, value)
  CheckConstraint:initialize(self)
  local ts = GetTilesByAttribute(attrName, comp, tostring(value))
  local buffer = {}
  for _, t in pairs(ts) do
    local circle = t:GetCircle(tileDistance)
    for _, ct in pairs(circle) do
      table.insert(buffer, ct)
    end
  end
  for _, bt in pairs(buffer) do
    table.insert(ts, bt)
  end

  self.keepAway = TileDataSet:new(ts)
  self.tileDistance = tileDistance
end

function MinDistanceFromAttribute:CheckTile(ti)
  if self.keepAway[ti] ~= nil then
    return false
  end
  return true
end


--------------- In Tile Range
InTileRange = class("InTileRange", SetConstraint)
function InTileRange:initialize(topLeft, bottomRight)
  self.topLeft = topLeft
  self.bottomRight = bottomRight
end

function InTileRange:GetPassingTiles()
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

function HasAttributes:GetPassingTiles()
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

function ConstraintSolver:Solve(debug)
  local tiles = nil
  for i, c in ipairs(self.constraints) do
    if (i == 1) then
      if c:isInstanceOf(SetConstraint) == false then
        io.stderr:write("LUA ERROR: First constraint is not a SetConstraint. Aborting solve.\n")
        return false
      end

      tiles = c:GetPassingTiles()
      if (debug) then
        print("initial tiles (" .. tostring(c) ..  ") | " .. tostring(#tiles:toList()) .. " tiles.")
      end
    else
      if (debug) then
        print("before iteration " .. tostring(i) .. "(" .. tostring(c) .. ") | " .. tostring(#tiles:toList()) .. " tiles.")
      end
      if c:isInstanceOf(SetConstraint) then
        tiles = c:FilterTiles(tiles)
      elseif c:isInstanceOf(CheckConstraint) then
        for ti, _ in pairs(tiles) do
          if not c:CheckTile(ti) then
            tiles[ti] = nil
          end
          end
        end
      if (debug) then
        print("\tafter: " .. tostring(#tiles:toList()) .. " tiles.")
      end
    end
  end

  local tList = tiles:toList()
  self.pickedTileSet = TileNEWSet()
  for _, t in ipairs(tList) do
    self.pickedTileSet = AddTileToSet(self.pickedTileSet, t)
  end
  if #tList > 0 then
    self.pickedTile = tList[RandomRangeInt(1, #tList)]
  end

  return true
end
