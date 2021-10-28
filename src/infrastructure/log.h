#pragma once

void Log(char* format, ...);
void LogErr(char* format, ...);

void RegisterLogFunction(void (*f)(char* output), int logType);
