#include "stdafx.h"

#define KGFLAGS_IMPLEMENTATION
#include <kgflags.h>

#include "platform/platform.h"
#include "infrastructure/rendering.h"
#include "scripting/scripting.h"
#include "infrastructure/attributes.h"
#include "infrastructure/game.h"

int main(int argc, char * argv[]) {
    const char* startupElement = NULL;
    kgflags_string("startup", "_Root", "The first element to push on to the HLVM stack.", false, &startupElement);
    
    if (!kgflags_parse(argc, argv)) {
        kgflags_print_errors();
        kgflags_print_usage();
        return 1;
    }
    
    InitializePlatform();
    InitializeRendering();
    InitializeLua();
    InitializeAttributes();

    InitializeGame(startupElement);

    bool shouldStop = false;
    while (!shouldStop) {
        bool updateStopping = GameTick();
        bool renderStopping = Render();
        shouldStop = updateStopping || renderStopping;
    }

    FinalizeGame();

    FinalizeAttributes();
    FinalizeLua();
    FinalizeRendering();
    FinalizePlatform();

    return EXIT_SUCCESS;

}
