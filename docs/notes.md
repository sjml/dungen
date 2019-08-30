## TODO
0. Styler rework
    - can it handle a region that is both water and plague?
1. Chamber/region/room model design
    - (see brainstorm notes)
    - somewhat more than tilesets
        - entering a tile needs to be equivalent to entering the chamber it belongs to
    - need to have triggers/encounters somehow
        - "Dwarves tunnel into chamber containing plague."
        - "Adventurers come into territory owned by demons."
2. Design and implement agents/entities
    - (update primordial wandering monsters and great wyrm to be agent)
    - individuals
        - attributes
        - ascribe agency to actions
    - civilizations
        - have zones
        - special case? or just an individual with a zone?
    - HtHaD calls both "groups"...
    - "Tunneling came to a region owned by demons; resolve conflict with $CURRENT_AGENT."
3. Relaxable constraint sets

## Tech Debt
* set precompiled headers up in Visual Studio
* typemaps should reject inputs that are not tables
* handle newlines in text rendering?
* figure out text extents ascender/descender issue
* add way to modify attributes instead of having to get/set round-trip through SQLite
* overusing the stretchy buffer right now; look for more chances to just use a static array
    - also not a huge deal, but I think the *free functions also NULL the pointer, so might have some redundant calls
    - also the type of the *len calls is ptrdiff_t, not necessarily long
* pull window dimensions from central spot instead of `1024.0f`-ing all over the place
* figure out why ScreenToWorld needs that multiplier at the end :-/ 

## Horizon
* in-game console
* HLVM exceptions (C + Lua invoking)
* pull sim settings and other stuff out into config file that Lua pulls in
* specify (and store) random seed
* let outlines specify inner/outer/center
* expose resources directory on mac?
* history recording -- optional live captioning
    - banner: "The Primordial Age"
    - little in-place captions "Natural caverns form with pockets of liquid hot magma" etc.
    - write all out to text and pretty formatting (or JSON+HTML renderer)
    - screenshots/video
    - name generator?
