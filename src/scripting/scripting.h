#pragma once

void InitializeLua(void);
void FinalizeLua(void);
lua_State* GetLuaState(void);

int RunFile(const char* filename);
int RunString(const char* string);
