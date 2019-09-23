#include "../stdafx.h"

#include <unistd.h>

void InitializePlatform() {
    if (chdir("./Resources") != 0) {
        fprintf(stderr, "ERROR: Could not find Resources directory. Exiting.\n");
        exit(EXIT_FAILURE);
    }
}

void FinalizePlatform() {
}
