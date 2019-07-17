## TODO
1. HLVM registers (C <-> Lua)
    - ints, floats, strings, tiles, tilesets
2. font rendering (C + Lua invoking)
    - just use easy_font for now; pull in truetype later when we feel like getting fancy
3. More declarative approach to coloring/rendering
    - have render resolver that is the only thing to apply colors
    - something akin to CSS (but hopefully less complicated)
4. pathfinding (in Lua?)
5. constraint solver (in Lua?)
6. choice presentation (tiles, die rolls)
    - means some kind of UI description from Lua, plus "wait for it" where VM stops processing momentarily
7. HLVM exceptions (C + Lua invoking)
8. player choice to overrule random tiles or die rolls (in Lua?)

## Horizon
* replace existing string split/munging code with SDS
* setup debugger in VS Code or ZeroBrane?
* specify (and store) random seed
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
