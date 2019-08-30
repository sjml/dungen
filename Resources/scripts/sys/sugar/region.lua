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
  return GetRegionAttributeInt(r, name)
end

rft.GetAttributeFloat = function(r, name)
  return GetRegionAttributeFloat(r, name)
end

rft.GetAttributeString = function(r, name)
  return GetRegionAttributeString(ts, name)
end

rft.CheckAttribute = function(r, name, comp, value)
  return false -- TODO: CheckRegionAttribute(td, name, comp, value)
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

rft.SetOutline = function(r, c, t)
  return SetRegionOutline(r, c, t)
end

rft.ClearOutline = function(r)
  return ClearRegionOutline(r)
end
