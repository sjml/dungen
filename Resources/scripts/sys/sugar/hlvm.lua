function quit(message)
  QuitGame(message)
end

function gsr(key)
  return GetStringRegister(key)
end

function gir(key)
  return GetIntRegister(key)
end

function gfr(key)
  return GetFloatRegister(key)
end

function gtr(key)
  return GetTileRegister(key)
end

function gtsr(key)
  return GetTileSetRegister(key)
end

function ssr(key, value)
  SetStringRegister(key, value)
end

function sir(key, value)
  SetIntRegister(key, value)
end

function sfr(key, value)
  SetFloatRegister(key, value)
end

function str(key, value)
  SetTileRegister(key, value)
end

function stsr(key, value)
  SetTileSetRegister(key, value)
end

