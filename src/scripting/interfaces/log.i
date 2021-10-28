%module dungen
%{
    #include "../infrastructure/log.h"
%}

void Log(char* format, ...);
void LogErr(char* format, ...);
