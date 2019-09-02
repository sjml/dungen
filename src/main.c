#include "stdafx.h"

#define KGFLAGS_IMPLEMENTATION
#include <kgflags.h>

#include "dungen.h"

int main(int argc, char * argv[]) {
    const char* startupElement = NULL;
    kgflags_string("startup", "_Root", "The first element to push on to the HLVM stack.", false, &startupElement);
    
    if (!kgflags_parse(argc, argv)) {
        kgflags_print_errors();
        kgflags_print_usage();
        return 1;
    }

    InitializeDunGen(startupElement);
    RunDunGen();
    FinalizeDunGen();

    return EXIT_SUCCESS;

}
