#include "stdafx.h"

#define KGFLAGS_IMPLEMENTATION
#include <kgflags.h>

#include "dungen.h"

static const char* startupElement;
void InitShim(void) {
    InitializeDunGen(startupElement);
}

sapp_desc sokol_main(int argc, char * argv[]) {
    kgflags_string("startup", "_Root", "The first element to push on to the HLVM stack.", false, &startupElement);

    if (!kgflags_parse(argc, argv)) {
        kgflags_print_errors();
        kgflags_print_usage();
        return (sapp_desc){0};
    }

    return (sapp_desc){
        .init_cb = InitShim,
        .frame_cb = TickAndRender,
        .cleanup_cb = FinalizeDunGen,
        .event_cb = EventCallback,

        .width = 1024,
        .height = 768,
        .window_title = "DunGen",
        .swap_interval = 1,
        .sample_count = 4,
        .high_dpi = true
    };
}
