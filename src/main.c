#include "stdafx.h"

#include "dungen.h"

static const char* startupElement;
void InitShim(void) {
    InitializeDunGen(startupElement);
}

sapp_desc sokol_main(int argc, char * argv[]) {
    sargs_setup(&(sargs_desc){
        .argc = argc,
        .argv = argv
    });

    if (sargs_exists("startup")) {
        startupElement = sargs_value("startup");
    }
    else {
        startupElement = "_Root";
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
