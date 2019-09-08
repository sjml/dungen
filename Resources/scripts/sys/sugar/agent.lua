local aft = GetFunctionTable("Agent")
aft.SetAttributeInt = function(a, name, value)
  SetAgentAttributeInt(a, name, value)
end

aft.SetAttributeFloat = function(a, name, value)
  SetAgentAttributeFloat(a, name, value)
end

aft.SetAttributeString = function(a, name, value)
  SetAgentAttributeString(a, name, value)
end

aft.GetAttributeInt = function(a, name)
  return math.floor(GetAgentAttributeInt(a, name))
end

aft.GetAttributeFloat = function(a, name)
  return GetAgentAttributeFloat(a, name)
end

aft.GetAttributeString = function(a, name)
  return GetAgentAttributeString(a, name)
end

aft.CheckAttribute = function(a, name, comp, value)
  return false -- TODO: CheckAgentAttribute(a, name, comp, value)
end

aft.AddTag = function(a, tag)
  AddAgentTag(a, tag)
end

aft.RemoveTag = function(a, tag)
  RemoveAgentTag(a, tag)
end

aft.HasTags = function(a, tagStr)
  return AgentHasTags(a, tagStr)
end

aft.GetTags = function(a)
  return GetAgentTags(a)
end
