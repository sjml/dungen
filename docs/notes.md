## TODO
1. Rearchitect HLVM stack
    - probably move more of it into Lua
    - Sandbox individual elements
        - don't let them declare globals, etc.
        - whitelist environment without yield, etc.
        - while we're here, figure out directory nesting for them
2. UI
    - choice presentation (tiles, die rolls)
    - lua can push a button list that waits for a response
        - if buttons have the same text, merge them
    - HLVM pauses while choices are up, resumes when button is clicked
        - this is going to be tricky -- how to handle this? some kind of "native push"? 
    - do something similar for picking a tile
    - also do banners with whatever pausing we figure out
3. player choice to overrule random tiles or die rolls (in Lua?)
4. HLVM exceptions (C + Lua invoking)

## Horizon
* pull dimensions from central spot instead of 1024.0f all over the place
* specify (and store) random seed
* let outlines specify inner/outer/center
* Windows build
    - Azure pipeline -> GitHub release (draft)
* let regions label themselves too
* history recording -- optional live captioning
    - banner: "The Primordial Age"
    - little in-place captions "Natural caverns form with pockets of liquid hot magma" etc.
    - write all out to text and pretty formatting (or JSON+HTML renderer)
    - screenshots/video
    - name generator?
* https://github.com/nothings/stb/blob/master/stb_leakcheck.h
* figure out why ScreenToWorld needs that multiplier at the end :-/ 
