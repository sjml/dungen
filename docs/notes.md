## TODO
1. choice presentation (tiles, die rolls)
    - means some kind of UI description from Lua, plus "wait for it" where VM stops processing momentarily
    - might mean a more "real" UI system :-/ 
2. constraint solver (in Lua?)
    - don't overdo it; just build what is needed as it is needed
3. HLVM exceptions (C + Lua invoking)
4. player choice to overrule random tiles or die rolls (in Lua?)

## Horizon
* specify (and store) random seed
* let outlines specify inner/outer/center
* let simulation elements be nested in directories
* Windows build
* better text rendering
    - then let regions label themselves too
* Azure pipeline -> GitHub release (draft)
* history recording -- optional live captioning
    - banner: "The Primordial Age"
    - little in-place captions "Natural caverns form with pockets of liquid hot magma" etc.
    - write all out to text and pretty formatting (or JSON+HTML renderer)
    - screenshots/video
    - name generator?
* whitelist environment for simulation elements (removing coroutine functions, for instance)
* https://github.com/nothings/stb/blob/master/stb_leakcheck.h
* take the pointer check code from old angel for vector typemaps
* figure out why ScreenToWorld needs that multiplier at the end :-/ 
