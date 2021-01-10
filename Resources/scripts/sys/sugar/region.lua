local rft = GetFunctionTable("Region")
rft.SetAttributeInt = function(r, name, value)
  SetRegionAttributeInt(r, name, value)
end

rft.SetAttributeFloat = function(r, name, value)
  SetRegionAttributeFloat(r, name, value)
end

rft.SetAttributeString = function(r, name, value)
  SetRegionAttributeString(r, name, value)
end

rft.GetAttributeInt = function(r, name)
  return math.floor(GetRegionAttributeInt(r, name))
end

rft.GetAttributeFloat = function(r, name)
  return GetRegionAttributeFloat(r, name)
end

rft.GetAttributeString = function(r, name)
  return GetRegionAttributeString(r, name)
end

rft.CheckAttribute = function(r, name, comp, value)
  -- <sigh> in the future, always put notes when doing something like
  --    this... why was this made false, past me? something with
  --    styling probably? what is the "DO" that I am "TO"?
  return false -- TODO: CheckRegionAttribute(r, name, comp, value)
end

rft.AddTag = function(r, tag)
  AddRegionTag(r, tag)
end

rft.RemoveTag = function(r, tag)
  RemoveRegionTag(r, tag)
end

rft.HasTags = function(r, tagStr)
  return RegionHasTags(r, tagStr)
end

rft.GetTags = function(r)
  return GetRegionTags(r)
end

rft.Destroy = function(r)
  return DestroyRegion(r)
end

rft.AddTile = function(r, t)
  return AddTileToRegion(r, t)
end

rft.RemoveTile = function(r, t)
  return RemoveTileFromRegion(r, t)
end

rft.GetTiles = function(r)
  return GetTilesFromSet(r.tiles)
end

rft.SetOutline = function(r, color, thickness, type)
  return SetRegionOutline(r, color, thickness, type)
end

rft.ClearOutline = function(r)
  return ClearRegionOutline(r)
end

rft.SetParent = function(r, parent)
  return SetRegionParent(r, parent)
end

rft.GetParent = function(r)
  return GetRegionParent(r)
end

rft.GetChildren = function(r)
  return GetRegionChildren(r)
end
