function exploreTile(targetTile, originTile, agent, isCarving)
  -- if !open
    -- if !carving
      -- return nil
    -- carve(target, from, agent)
  -- checkForEncounters(targetTile, originTile, agent)
    -- if tile is part of an agent's domain:
      -- resolve encounter with other agent
      -- if this agent is now dead, return nil
    -- if tile is part of chamber
      -- resolve whatever happens in the chamber and pick exit point
      -- return exit point or nil
    -- do any last checks for individual tile stuff
      -- return targetTile
end
