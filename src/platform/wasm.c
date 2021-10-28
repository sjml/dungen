#include "../stdafx.h"

#include <unistd.h>

#include "../infrastructure/log.h"

void InitializePlatform() {
    if (chdir("./Resources") != 0) {
        LogErr("ERROR: Could not find Resources directory. Exiting.\n");
        exit(EXIT_FAILURE);
    }
}

void FinalizePlatform() {
}

sds GetShaderPath(const char* shaderName) {
    char* path = "shaders/glsl300es/";
    char* suffix = ".glsl";

    return sdscatfmt(sdsempty(), "%s%s%s", path, shaderName, suffix);;
}
