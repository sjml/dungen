#include "stdafx.h"
#include "log.h"



#define MAX_LOG_FUNCTIONS 8

static int numLogFunctions = 0;
static void(*logFunctionPointers[MAX_LOG_FUNCTIONS])(char*);
static int numErrorFunctions = 0;
static void(*errorFunctionPointers[MAX_LOG_FUNCTIONS])(char*);

void Log(char* format, ...) {
    va_list argp;
    va_start(argp, format);
    sds outString = sdscatvprintf(sdsempty(), format, argp);
    va_end(argp);

    fputs(outString, stdout);

    for (int i=0; i < numLogFunctions; i++) {
        (*logFunctionPointers[i])(outString);
    }

    sdsfree(outString);
}

void LogErr(char* format, ...) {
    va_list argp;
    va_start(argp, format);
    sds outString = sdscatvprintf(sdsempty(), format, argp);
    va_end(argp);

    fputs(outString, stderr);

    for (int i=0; i < numErrorFunctions; i++) {
        (*errorFunctionPointers[i])(outString);
    }

    sdsfree(outString);
}

void RegisterLogFunction(void (*f)(char* output), int logType) {
    if (logType == 0) {
        if (numLogFunctions == MAX_LOG_FUNCTIONS) {
            LogErr("ERROR: Log function maximum already reached.");
            return;
        }
        logFunctionPointers[numLogFunctions++] = f;
    }
    else if (logType == 1) {
        if (numErrorFunctions == MAX_LOG_FUNCTIONS) {
            LogErr("ERROR: Error function maximum already reached.");
            return;
        }
        errorFunctionPointers[numErrorFunctions++] = f;
    }
}
