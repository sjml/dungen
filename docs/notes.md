## TODO
1. Tech debt: 
    - null checks in C functions
    - make a screentoworld function
    - have world setup wrapped in a transaction?
    - replace existing string split/munging code with SDS
        - also use const char* where appropriate
    - cache SQL statements so they don't need to be recompiled afresh
    - fix randomness (gb_random* funcs all use static seed)
    - find a consistent spot to call ResolveStyles()
    - unify ApplyFillStyle and ApplyOutlineStyle
    - specify outline thickness
    - take the pointer check code from old angel for vector typemaps
2. constraint solver (in Lua?)
3. pathfinding (in Lua?)
4. choice presentation (tiles, die rolls)
    - means some kind of UI description from Lua, plus "wait for it" where VM stops processing momentarily
5. HLVM exceptions (C + Lua invoking)
6. player choice to overrule random tiles or die rolls (in Lua?)

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
