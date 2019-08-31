#include "stdafx.h"

#include <unistd.h>

void InitializePlatform() {
    #if DEBUG
        chdir("../Resources");
    #endif
}

void FinalizePlatform() {
}
