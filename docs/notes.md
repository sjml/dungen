## TODO
1. More declarative approach to coloring/rendering
    - NB: get tileset by index needed
    - labels
2. Tech debt: 
    - null checks in C functions
    - have world setup wrapped in a transaction?
    - replace existing string split/munging code with SDS
    - cache SQL statements so they don't need to be recompiled afresh
    - fix randomness (gb_random* funcs all use static seed)
    - find a consistent spot to call ResolveStyles()
    - unify ApplyFillStyle and ApplyOutlineStyle
3. constraint solver (in Lua?)
4. pathfinding (in Lua?)
5. choice presentation (tiles, die rolls)
    - means some kind of UI description from Lua, plus "wait for it" where VM stops processing momentarily
6. HLVM exceptions (C + Lua invoking)
7. player choice to overrule random tiles or die rolls (in Lua?)

## Horizon
* setup debugger in VS Code or ZeroBrane?
* specify (and store) random seed
* let outlines specify inner/outer/center
* let simulation elements be nested in directories
* Windows build
* Azure pipeline -> GitHub release (draft)
* history recording -- optional live captioning
    - banner: "The Primordial Age"
    - little in-place captions "Natural caverns form with pockets of liquid hot magma" etc.
    - write all out to text and pretty formatting (or JSON+HTML renderer)
    - screenshots/video
    - name generator?
* whitelist environment for simulation elements (removing coroutine functions, for instance)
* https://github.com/nothings/stb/blob/master/stb_leakcheck.h
